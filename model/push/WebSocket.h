/**
 * file :	WebSocket.h
 * author :	bushaofeng
 * create :	2014-10-05 23:26
 * func : 
 * history:
 */

#ifndef	__WEBSOCKET_H_
#define	__WEBSOCKET_H_

#include <iostream>
#include <map>
#include <sys/time.h>
#include <time.h>
#include "bs.h"
#include "AsyncSocket.h"
//#include "hiredis.h"
#define DELAY_TIME 20

using namespace std;
typedef enum {
    OPCODE_CONTINUE = 0x00,
    OPCODE_TEXT = 0x01,
    OPCODE_BINARY = 0x02,
    // 保留非控制帧
    OPCODE_UNCONTROL = 0X03,
    OPCODE_CLOSE = 0x08,
    OPCODE_PING = 0x09,
    OPCODE_PONG = 0x0a,
    OPCODE_UNEXIST = 0x0c,
    OPCODE_OK = 0x0d,
    OPCODE_ERROR = 0x0e,
    // 保留控制帧
    OPCODE_CONTROL = 0x0b,
} ws_opcode_t;

typedef enum{
    // 状态码
    STATUS_CLOSE    = 0x00,
    STATUS_OPENING  = 0x01,
    STATUS_CONNECTED= 0x02,
    STATUS_ERROR    = 0x03,
    STATUS_UNEXIST  = 0x04,
}ws_status_t;

typedef enum{
    WS_RESPONSE_INVALID = 0,
    WS_RESPONSE_OK = 101,
    WS_RESPONSE_NOTFOUND = 400
}ws_response_code_t;

typedef struct ws_protocal_t {
    uint8_t fin:1;      // 是否还有数据
    uint8_t rsv:3;      // 保留
    uint8_t opcode:4;   // 消息类型
    uint8_t mask:1;     // 掩码处理，如果是1，Masking-key域的数据即是掩码密钥
    uint8_t payload_len:7;  // playload_len=126, 使用16位extern len，127使用64位extern len
    union {
        uint16_t payload_len_16;
        uint64_t payload_len_64;
    };
    uint32_t masking_key;
    const char *payload;
    const char *buffer;
} ws_protocol_t;

/////// ios上类生命编译不通过，提示重复定义错误，所以将WebSocketListener类放入WebSocket中
class WebSocket : public AsyncSocket {
public:
    // 如果sock为0，则open函数重新创建socket
    WebSocket(int sock);

    ~WebSocket() {
        close(m_sock);
        bs_conf_destroy(&m_handshake);
        delete m_write_buffer;
    }

    // async socket的回调
    virtual void onWrite() {}
    virtual void onError(int error);
    virtual void onMessage(async::message_t *msg);

    // websocket的回调
    virtual void opened() = 0;
    virtual void message(const char* msg, uint32_t size) = 0;
    virtual void error() = 0;
    virtual void cloesed() = 0;

    state_t open(const char *url);

    int sendText(const void *buf, uint32_t size);
    int sendBinary(const void *buf, uint32_t size);
    int send(int type, const char *buf, uint32_t size);

    int getStatus() { return m_status; }
    int getError() { return m_error; }
    uint64_t getPingTime() { return m_pingtime; }
    uint64_t getPongTime() { return m_pongtime; }

    void setStatus(uint8_t status) { m_status = status; }
    void setPingTime(uint64_t pingtime) { m_pingtime = pingtime; }

protected:
    state_t parseRequest(const char *buf, uint32_t size);
    state_t responseHandshake(int code=101);
    ws_response_code_t parseResponse(const char *buf, uint32_t size);

    //int push(int type, const char* buf, uint32_t size);
    // 最多只能填mtu-8个大小
    uint32_t frame(uint8_t fin, uint8_t opcode, uint32_t mask, const char *buf, uint32_t size);
    state_t parseFrame(ws_protocal_t *protocal, const char *buf, uint32_t size);
    uint64_t protocalSize();
    uint64_t protocalBuffer(char *buffer, uint64_t size);

    //记录ping，pong的时间
    uint64_t    m_pongtime;
    uint64_t    m_pingtime;

    map<string, string> m_headers;
    uint8_t     m_status;
    int         m_error;
    data_t      m_read_buffer;
    char *m_write_buffer;
};
#endif
