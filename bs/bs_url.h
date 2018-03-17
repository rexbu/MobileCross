/**
 * file :	bs_url.h
 * author :	bushaofeng
 * create :	2014-10-06 23:00
 * func : 
 * history:
 */

#ifndef	__BS_URL_H_
#define	__BS_URL_H_

#include "bs_type.h"
#include "bs_common.h"
#include "bs_conf.h"
#include "bs_object.h"
#include "bs_data.h"

#define URL_SIZE    4096

#define HTTP_INVALID    -1
#define HTTP_OK         200
#define HTTP_NOTFOUND   404

#ifdef __cplusplus
extern "C"{
#endif

typedef enum{
    HTTP_GET,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE
}http_method_t;
    
typedef struct url_t{
    object_t    base;
    uint16_t    port;
    //char        url[URL_SIZE];
    data_t      url;
    data_t      protocal;
    data_t      host;
    data_t      domain;
    data_t      path;
    data_t      query;
}url_t;
    
typedef struct http_res_header{
    uint16_t status_code;           // HTTP/1.1 '200' OK
    char content_type[128];         // Content-Type: application/gzip
    uint64_t content_length;        // Content-Length: 11683079
}http_res_header;

void* url_init(void* p);
void url_destroy(void* p);
state_t url_parse(url_t* url, const char* url_str);
void url_print(url_t* url);

typedef struct http_res_t http_res_t; 
typedef struct http_t{
    url_t       url;    // url中已经有base，不需要再有
    data_t      req;
    char*       body;
    uint32_t    body_size;
    uint32_t    time_out;
}http_t;

void* http_init(void* p);
void http_destroy(void* p);
/// 创建http
http_t* http_create(const char* url, const char* method);
/// 设置http头
void http_set_header(http_t* http, const char* key, const char* value);
/// http头增加新的行
void http_add_newline(http_t* http);
/// 设置http body
void http_set_body(http_t* http, const char* body, uint32_t body_size);
/// 执行http，内部会delete http结构体
http_res_t* http_perform(http_t* http);
/// 下载文件进度回调
typedef void (*http_progress)(uint64_t writen, uint64_t total_writen, uint64_t total_expect_write, void* download);
/// 下载文件到本地. return ==0:正确；>0:http错误码 <0:连接http失败
http_res_t* http_download(http_t* http, const char* file, void* download, http_progress progressCallback);
/// 上传文件到服务器。return ==0:正确；>0:http错误码 <0:连接http失败
http_res_t* http_upload(http_t* http, const char* path, void* upload, http_progress progressCallback);
/// 上传二进制数据到服务器。return ==0:正确；>0:http错误码 <0:连接http失败
http_res_t* http_post_data(http_t* http, uint8_t* data, void* upload, http_progress progressCallback);
/// http request请求
void http_send_request(int socket, http_res_t* http_res, http_t* http);
/// http response接收
void http_receive_response(int socket, http_res_t* http_res, http_t* http);
/// http response header接收
void http_receive_header(int socket, http_res_t *http_res, http_t* http);

struct http_res_t{
    data_t      response;
    int         response_code;
    char*       body;
    uint32_t    body_size;
    char*       cookie;
};

void* http_res_init(void *p);
void http_res_destroy(void *p);

state_t connect_socket(int *sock, http_t *http);
state_t http_response_parse(http_res_t *res);
state_t read_timeout(int socket, uint32_t wait_seconds);
state_t write_timeout(int socket, uint32_t wait_seconds);
state_t connect_timeout(int socket, uint32_t wait_seconds);
#ifdef __cplusplus
}
#endif

#endif
