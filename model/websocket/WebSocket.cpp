//
// Created by Rex on 2018/3/17.
//

#include <basic.h>
#include "WebSocket.h"

static int callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in,
                    size_t len);
static void* on_message(void* msg);

static struct lws_protocols protocols[] = {
    {"chat", callback, sizeof(void*), 0},
    {NULL, NULL, 0, 0} // end of list
};

WebSocket* WebSocket::m_instance = NULL;

WebSocket::WebSocket(const char* ca, const char* cert, const char* cert_key){
    memset(&m_create_info, 0, sizeof(lws_context_creation_info));
    memset(&m_conn_info, 0, sizeof(lws_client_connect_info));
    m_instance = this;

    if (ca!=NULL && cert!=NULL && cert_key!=NULL){
        m_create_info.ssl_cert_filepath = ca;
        m_create_info.ssl_ca_filepath = cert;
        m_create_info.ssl_private_key_filepath = cert_key;
        m_use_ssl = LCCSCF_USE_SSL | LCCSCF_ALLOW_SELFSIGNED | LCCSCF_SKIP_SERVER_CERT_HOSTNAME_CHECK;
    }
    else{
        m_create_info.ssl_cert_filepath = NULL;
        m_create_info.ssl_ca_filepath = NULL;
        m_create_info.ssl_private_key_filepath = NULL;
        m_use_ssl = 0;
    }

    m_status = WS_STATUS_UNCONNECTED;
    create(ca, cert, cert_key);
}

void WebSocket::create(const char* ca, const char* cert, const char* cert_key){
    m_create_info.port = CONTEXT_PORT_NO_LISTEN;
    protocols[0].user = this;
    m_create_info.protocols = protocols;
    m_create_info.gid = -1;
    m_create_info.uid = -1;
    m_create_info.timeout_secs = 10;
    m_create_info.ws_ping_pong_interval = 20;
    m_create_info.options = 0;

    lws_set_log_level(LLL_NOTICE | LLL_INFO | LLL_ERR | LLL_WARN | LLL_CLIENT, NULL);
    // 初始化长链接
    m_context = lws_create_context(&m_create_info);
    if (m_context==NULL){
        err_log("create websocket context error!");
    }
}

bool WebSocket::connect(const char* host, int port, const char* path){
    if (host!=NULL && path!=NULL){
        m_host = host;
        m_port = port;
        m_path = path;
    }

    m_conn_info.port = m_port;
    m_conn_info.address = m_host.c_str();
    m_conn_info.path = m_path.c_str();
    m_conn_info.context = m_context;
    m_conn_info.ssl_connection = m_use_ssl;
    m_conn_info.host = m_host.c_str();
    m_conn_info.origin = m_host.c_str();
    m_conn_info.ietf_version_or_minus_one = -1;
    m_conn_info.client_exts = NULL;
    m_conn_info.protocol = protocols[0].name;

    m_lws = lws_client_connect_via_info(&m_conn_info);
    if (m_lws == NULL) {
        // Error
        err_log("jni_connectLws failed");
        return false;
    }

    m_status = WS_STATUS_CONNECTING;
    info_log("jni_connectLws start");
    return true;
}

WebSocket::~WebSocket(){
    exit();
}

bool WebSocket::exit(){
    stop();
    lws_cancel_service(m_context);
    lws_context_destroy(m_context);
    return true;
}

int WebSocket::send(const char* msg, uint32_t size){
    if (size==0 && msg!=NULL){
        size = strlen(msg);
    }

    unsigned char* s = (unsigned char*)calloc(1, size+2*LWS_SEND_BUFFER_PRE_PADDING+1);
    memcpy(s+LWS_SEND_BUFFER_PRE_PADDING, msg, size);
    int re = lws_write(m_lws, s+LWS_SEND_BUFFER_PRE_PADDING, size, LWS_WRITE_TEXT);
    free(s);
    return re;
}

bool WebSocket::alive(){
    return lws_send_pipe_choked(m_lws)==0;
}
void WebSocket::pingpong(){
}

void WebSocket::loop(){
    lws_service(m_context, 20000);
    // 如果已经断开则重新连接
    if (!alive() && m_status==WS_STATUS_UNCONNECTED){
        err_log("not alive, reconnect %s:%d%s", m_host.c_str(), m_port, m_path.c_str());
        connect();
    }
}

static int callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    if (reason == LWS_CALLBACK_GET_THREAD_ID) {
        return 0;
    }

    //TODO: 没搞明白如何用user带入参数
    WebSocket* ws = WebSocket::m_instance;

    switch (reason){
        case LWS_CALLBACK_CLIENT_RECEIVE: {
            // 消息异步处理，防止阻塞
            char* msg = (char*)calloc(1, len+1);
            memcpy(msg, in ,len);
            async_run(on_message, msg);
            break;
        }
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            ws->setStatus(WS_STATUS_CONNECTED);
            ws->opened();
            break;
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            ws->error();
            break;
        case LWS_CALLBACK_CLOSED:
            ws->setStatus(WS_STATUS_UNCONNECTED);
            ws->cloesed();
            break;
        case LWS_CALLBACK_CLIENT_WRITEABLE:
            break;

        default:
            info_log("no parser command:%d ", reason);
            break;
    }

    return 0;
}

void* on_message(void* msg){
    WebSocket* ws = WebSocket::m_instance;
    uint32_t size = strlen((char*)msg);
    ws->message((const char*)msg, size);
    free(msg);
    return NULL;
}