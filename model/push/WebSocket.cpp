	/**
 * file :	WebSocket.cpp
 * author :	bushaofeng
 * create :	2014-10-05 23:26
 * func : 
 * history:
 */

#include "bs.h"
#include "WebSocket.h"
#include "SocketFrame.h"
#include <vector>
#include <string.h>

static void print_protocal(ws_protocal_t* protocal);

WebSocket::WebSocket(WebSocketListener* listener, int sock):
AsyncSocket(sock, SOCK_STREAM){
    cqueue_init(&m_read_session);
    // mtu必须大于126，方便分包
    assert(m_mtu>126 && m_mtu<0xffff);
    m_write_buffer = new char[m_mtu];
    assert(m_write_buffer);
    m_status = STATUS_CLOSE;
    
    m_pingtime = 0;
    m_pongtime = 0;
    if (sock==0) {
        m_sock = socket_tcp(BS_FALSE);
    }
}

state_t WebSocket::open(const char* url_str,const char* sessionID){
    state_t     st;
    url_t       url;
    char        domain[URL_SIZE] = {0};
    char        hand[SOCKET_TCP_MTU] = {0};
    
    url_parse(&url, url_str);
    bs_memcpy(domain, URL_SIZE, url.domain, url.domain_size);
    if (m_status != STATUS_CONNECTED) {
        m_sock = socket_tcp(BS_FALSE);
    }
        st = bs_sock_connect(m_sock, domain, url.port);
    
    if (st != BS_SUCCESS) {
        err_log("connect[%s:%d] error[%d]", domain, url.port, st);
        m_status = STATUS_ERROR;
        return st;
    }
    
    socket_unblock(m_sock);
    //把sessionID添加进去  客户端在此发送，服务端接收，修改协议，添加sessionID，然后在服务端解析
    snprintf(hand, SOCKET_TCP_MTU, "GET %s HTTP/1.1\r\nHost: %s\r\nUpgrade: websocket\r\nConnection: upgrade\r\nSec-WebSocket-Key: key\r\nSec-WebSocket-Protocol: live\r\nSec-WebSocket-Version: 17\r\n",
             url.path, url.host);
    debug_log("shake message[%lu]: %s", strlen(hand), hand);

    send(m_sock, hand, strlen(hand), 0);
    m_status = STATUS_OPENING;
    
    return BS_SUCCESS;
}

void WebSocket::onError(int error){
    // 如果是close socket引起的error忽略
    if (m_status == STATUS_CLOSE) {
        return;
    }
    err_log("socket[%d] error[%d]", m_sock, error);
    m_status = STATUS_ERROR;
    m_error = error;
    close(m_sock);
}

void WebSocket::onMessage(async::message_t* msg){
    char*       buf = msg->buf;
    uint32_t    size = msg->size;
    
    buf[size] = '\0';
    //服务端，来自客户端的握手需要校验sessionID合法性，接收了修改之后的协议
    if (memcmp(buf, "GET", 3) == 0) {
        debug_log("handshake [%s]", buf);
        if(parseRequest((const char*)buf, size) == BS_SUCCESS){
            opened();
        }
        else{
            responseHandshake(400);
        }
        bs_delete(msg);
    }
    //客户端，来自服务端的握手
    else if (memcmp(buf, "HTTP/1.1", strlen("HTTP/1.1")) == 0){
        ws_response_code_t res = parseResponse((const char*)buf, size);
    
        if (res != WS_RESPONSE_OK) {
            debug_log("login in fail: %d", res);
            cloesed();
        }
        else
        {
            debug_log("login in success");
            m_status = STATUS_CONNECTED;
            //登陆成功，开始发送心跳ping
            push(OPCODE_PING, NULL, 0);
            bs_delete(msg);
        }
    }
    else{
        struct timeval tv;
        struct timezone tz;
        
        long pingtime;
        
        ws_protocal_t   protocal;
        state_t         st;
        uint64_t        buffer_size;
        char*           buffer;
        
        st = parseFrame(&protocal, (const char*)buf, size);
        if (st != BS_SUCCESS) {
            push(OPCODE_ERROR, NULL, 0);
            pool_free(&m_read_pool, buf);
            return;
        }
        
        print_protocal(&protocal);
        cqueue_push(&m_read_session, &protocal);
        // 如果Frame框架有多个工作线程，这里可能会有问题
        if (protocal.fin) {
            switch (protocal.opcode) {
                case OPCODE_CONTINUE:
                    break;
                case OPCODE_TEXT:
                    buffer_size = protocalSize();
                    buffer = new char[buffer_size];
                    protocalBuffer(buffer, buffer_size);
                    m_listener->onText(this, buffer, buffer_size);
                    push(OPCODE_OK, NULL, 0);
                    delete[] buffer;
                    break;
                case OPCODE_BINARY:
                    buffer_size = protocalSize();
                    buffer = new char[buffer_size];
                    protocalBuffer(buffer, buffer_size);
                    m_listener->onBinary(this, buffer, buffer_size);
                    push(OPCODE_OK, NULL, 0);
                    delete[] buffer;
                    break;
                case OPCODE_PING:
                {
                    //服务端收到客户端的ping消息，返回pong
                    //记录接收时间
                    gettimeofday (&tv , &tz);
                    pingtime = tv.tv_sec;
                    char str[20];
                    sprintf(str,"%ld",pingtime);
                    m_listener->setPingTime(getUserId(), str);
                    buffer_size = protocalSize();
                    buffer = new char[buffer_size];
                    protocalBuffer(buffer, buffer_size);
                    state_t st = m_listener->onExist(getUserId());
                    if (getUserId()!= NULL && st == BS_NOTFOUND) {
                        push(OPCODE_UNEXIST, NULL, 0);
                        m_status = STATUS_UNEXIST;
                        break;
                    }
                    push(OPCODE_PONG, NULL, 0);
                    m_status = STATUS_CONNECTED;
                    delete []buffer;
                    break;
                }
                case OPCODE_PONG:
                    //客户端收到服务端发送的pong
                    buffer_size = protocalSize();
                    buffer = new char[buffer_size];
                    protocalBuffer(buffer, buffer_size);
                    m_status = STATUS_CONNECTED;
                    debug_log("I'm alive");
                    //记录接收时间
                    gettimeofday (&tv , &tz);
                    m_pongtime = uint64_t(tv.tv_sec);
                    delete []buffer;
                    break;
                case OPCODE_UNEXIST:
                    //客户端收到服务端发送的sessionid无效
                    buffer_size = protocalSize();
                    buffer = new char[buffer_size];
                    protocalBuffer(buffer, buffer_size);
                    m_status = STATUS_UNEXIST;
                    delete []buffer;
                    break;
                case OPCODE_CLOSE:
                    m_status = STATUS_CLOSE;
                    m_listener->onClose(this);
                    close(m_sock);
                    break;
                case OPCODE_OK:
                    buffer_size = protocalSize();
                    buffer = new char[buffer_size];
                    protocalBuffer(buffer, buffer_size);
                    //删除消息
                    m_listener->onSendOK(getUserId());
                    delete []buffer;
                    break;
                case OPCODE_ERROR:
                    buffer_size = protocalSize();
                    buffer = new char[buffer_size];
                    protocalBuffer(buffer, buffer_size);
                    //重新发送
                    m_listener->onResend(getUserId());
                    delete []buffer;
                    break;
                default:
                    break;
            }
        }
    }
    buf = NULL;
}

uint64_t WebSocket::protocalSize(){
    uint64_t    size = 0;
    
    for (uint32_t pos = m_read_session.head; pos != m_read_session.rear; pos=cqueue_next(&m_read_session, pos)) {
        ws_protocal_t* protocal = (ws_protocal_t*)cqueue_get(&m_read_session, pos);
        if (protocal->payload_len < 126) {
            size += (protocal->payload_len);
        }
        else if (protocal->payload_len == 126){
            size += (protocal->payload_len_16);
        }
        else {
            size += (protocal->payload_len_64);
        }
    }
    
    return size;
}

uint64_t WebSocket::protocalBuffer(char* buffer, uint64_t size){
    ws_protocal_t       protocal;
    uint64_t            total_size = 0;
    
    while (cqueue_pop(&m_read_session, &protocal)==BS_SUCCESS) {
        if (protocal.payload_len < 126) {
            memcpy(buffer+total_size, protocal.payload, protocal.payload_len);
            total_size += (protocal.payload_len);
        }
        else if (protocal.payload_len == 126){
            memcpy(buffer+total_size, protocal.payload, protocal.payload_len_16);
            total_size += (protocal.payload_len_16);
        }
        else {
            memcpy(buffer+total_size, protocal.payload, protocal.payload_len_64);
            total_size += (protocal.payload_len_64);
        }
        
        pool_free(&m_read_pool, (void*)protocal.buffer);
    }
    
    return total_size;
}

status_t WebSocket::parseRequest(const char* buf, uint32_t size){
    char    key[64] = {0};
    char    value[256] = {0};
    char*   ptr = (char*)buf;

    if (memcmp(ptr, "GET", 3) == 0 && strstr(ptr, "HTTP/1.1")!=NULL) {
        m_headers['method'] = 'GET';
        sscanf(ptr, "GET %s HTTP/1.1", value);
        m_headers['path'] = value;
        m_headers['http'] = "1.1";
    }
    else{
        return WS_RESPONSE_INVALID;
    }
    
    ptr = strstr(ptr, "\r\n");
    while (ptr != NULL && ptr-buf<size-2) {
        sscanf(ptr, "\r\n%s %s\r\n", key, value);
        key[strlen(key)-1] = '\0';
        //value[strlen(value)-1] = '\0';
        //debug_log("key[%s] value[%s]", key, value);
        m_headers[string(key)] = string(value);
        ptr = strstr(ptr+2, "\r\n");
    }
    
    return BS_SUCCESS;
}

ws_response_code_t WebSocket::parseResponse(const char* buf, uint32_t size){
    char    key[64];
    char    value[256];
    int     response_code;
    char*   ptr = (char*)buf;
        
    if (memcmp(ptr, "HTTP/1.1",strlen("HTTP/1.1")) == 0){
        sscanf(ptr, "HTTP/1.1 %d Switching Protocols\r\n", &response_code);
        debug_log("ResponseCode: %d", response_code);
    }
    else{
        return WS_RESPONSE_INVALID;
    }
    
    ptr = strstr(ptr, "\r\n");
    while (ptr != NULL && ptr-buf<size-2) {
        sscanf(ptr, "\r\n%s %s\r\n", key, value);
        key[strlen(key)-1] = '\0';
        m_headers[string(key)] = string(value);
        ptr = strstr(ptr+2, "\r\n");
    }
    
    return response_code;
}

state_t WebSocket::responseHandshake(int code){
    char        buf[1024];
    
    snprintf(buf, 1024, "HTTP/1.1 %d Switching Protocols\r\n\
             Connection: Upgrade\r\n\
             Server: %s\r\n\
             Upgrade: WebSocket\r\n\
             Date: \r\n\
             Access-Control-Allow-Credentials: true\r\n\
             Access-Control-Allow-Headers: content-type\r\n\
             Sec-WebSocket-Accept: \r\n", code, "push.me-yun.com");
    return send(m_sock, buf, strlen(buf), 0)>0 ? BS_SUCCESS:BS_INVALID;
}

state_t WebSocket::sendBinary(const void* buf, uint32_t size){
    return send(OPCODE_BINARY, (const char*)buf, size)>0 ? BS_SUCCESS:BS_SENDERR;
}

state_t WebSocket::sendText(const void* buf, uint32_t size){
    return send(OPCODE_TEXT, (const char*)buf, size)>0 ? BS_SUCCESS:BS_SENDERR;
}

int WebSocket::send(int opcode, const char* buf, uint32_t size){
    uint32_t            buffer_size;
    
    // m_mtu>126 && m_mtu<0xffff
    if (size <= m_mtu-8) {
        buffer_size = frame(1, opcode, 0, buf, size);
        return (int)send(m_sock, m_write_buffer, buffer_size, 0);
    }
    else if(size > m_mtu-8){
        uint32_t    total_size = 0;
        
        while (total_size < size) {
            if (size-total_size>m_mtu-8) {
                buffer_size = frame(0, opcode, 0, buf+total_size, m_mtu-8);
                total_size += m_mtu - 8;
            }
            else{
                buffer_size = frame(1, opcode, 0, buf+total_size, size-total_size);
                total_size += (size - total_size);
            }
            if (send(m_sock, m_write_buffer, buffer_size, 0)<=0){
                err_log("send error. size[%d]", buffer_size);
                return -1;
            }
        }
    }
    
    return size;
}

uint32_t WebSocket::frame(uint8_t fin, uint8_t opcode, uint32_t mask_key, const char* buf, uint32_t size){
    ws_protocal_t       protocal;
    
    if (size > m_mtu-8){
        return 0;
    }
    
    memset(&protocal, 0, sizeof(ws_protocal_t));
    protocal.fin = fin;
    protocal.opcode = opcode;
    protocal.mask = 1;
    protocal.masking_key = mask_key;
    // m_mtu>126 && m_mtu<0xffff, size不会大于uint32_t最大值，所以payload_len不会为127
    // 暂时无Extension data， payload_len就是Application data长度
    if (size<126) {
        protocal.payload_len = size;
        // 小于126，头长度为
        memcpy(m_write_buffer, &protocal, 2);
        memcpy(m_write_buffer+2, &protocal.masking_key, 4);
        memcpy(m_write_buffer+6, buf, size);
        return size+6;
    }
    else if(size<65535){
        protocal.payload_len = 126;
        protocal.payload_len_16 = size;
        memcpy(m_write_buffer, &protocal, 4);
        memcpy(m_write_buffer+4, &protocal.masking_key, 4);
        memcpy(m_write_buffer+8, buf, size);
        return size+8;
    }
    else{
        protocal.payload_len = 126;
        protocal.payload_len_16 = size;
        memcpy(m_write_buffer, &protocal, 4);
        memcpy(m_write_buffer+4, &protocal.masking_key, 4);
        memcpy(m_write_buffer+8, buf, size);
        return size+8;
    }
    
    return 0;
}

state_t WebSocket::parseFrame(ws_protocal_t* protocal, const char* buf, uint32_t size){
    // 暂时无Extension data， payload_len就是Application data长度
    memcpy(protocal, buf, sizeof(ws_protocal_t));
    if (protocal->payload_len<126) {
        protocal->masking_key = *(uint32_t*)(buf+2);
        protocal->payload = buf+6;
        protocal->buffer = buf;
        if (size != protocal->payload_len+6) {
            return BS_PARAERR;
        }
    }
    else if(protocal->payload_len == 126){
        protocal->payload_len_16 = *(uint16_t*)(buf+2);
        protocal->masking_key = *(uint32_t*)(buf+4);
        protocal->payload = buf+8;
        protocal->buffer = buf;
        if (size != protocal->payload_len_16+8) {
            return BS_PARAERR;
        }
    }
    else if(protocal->payload_len == 127){
        protocal->payload_len_64 = *(uint64_t*)(buf+2);
        protocal->masking_key = *(uint32_t*)(buf+10);
        protocal->payload = buf+14;
        protocal->buffer = buf;
        if (size != protocal->payload_len_64+14) {
            return BS_PARAERR;
        }
    }
    
    return BS_SUCCESS;
}

void print_protocal(ws_protocal_t* protocal){
    debug_log("fin[%d] opcode[%d] mask[%d] payload_len[%d/%u/%llu] masking_key[%d]",
              protocal->fin, protocal->opcode, protocal->mask,
              protocal->payload_len, protocal->payload_len_16, protocal->payload_len_64,
              protocal->masking_key);
}
