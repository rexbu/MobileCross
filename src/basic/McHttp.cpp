/**
 * file :	McHttp.cpp
 * author :	Rex
 * create :	2016-11-11 14:41
 * func : 
 * history:
 */

#include "bs.h"
#include "McDevice.h"
#include "McHttp.h"
#include "McFile.h"
#include "McTask.h"
#include "ThreadPool.h"

using namespace mc;

#pragma --mark "http结构体封装"
typedef struct{
    object_t        base;
    http_t*         http;
    HttpCallback*   http_callback;
}mc_http_t;

extern "C" void* mc_http_init(void* h){
    mc_http_t* http = (mc_http_t*)h;
    http->http = NULL;
    http->http_callback = NULL;
    return http;
}

extern "C" void mc_http_destroy(void* h){
}

typedef struct{
    object_t        base;
    http_t*         http;
    std::string     path;
    DownCallback*   down_callback;
}mc_down_t;

extern "C" void* mc_down_init(void* p){
    mc_down_t* down = (mc_down_t*)p;
    down->http = NULL;
    down->down_callback = NULL;
    return down;
}
extern "C" void mc_down_destroy(void* p){
    
}

#pragma --mark "HttpSession 定义"
// 数据获取函数
void* thread_http(void* para);
void* thread_down(void* para);

HttpSession::HttpSession(void* threadpool){
    if (threadpool==NULL) {
        m_thread_pool = ThreadPool::shareInstance();
    }
    else{
        m_thread_pool = threadpool;
    }
}

HttpSession::~HttpSession(){
}

void HttpSession::get(const char* url, HttpCallback* callback){
    http(url, "GET", NULL, 0, callback);
}

void HttpSession::post(const char* url, const char* body, uint32_t length , HttpCallback* callback){
    http(url, "POST", body, length, callback);
}

void HttpSession::put(const char* url, const char* body, uint32_t length, HttpCallback* callback){
    http(url, "PUT", body, length, callback);
}

void HttpSession::del(const char* url, HttpCallback* callback){
    http(url, "DELETE", NULL, 0, callback);
}

void HttpSession::http(const char* url, const char* method, const char* body, uint32_t length, HttpCallback* callback){
    mc_http_t* h = bs_new(mc_http);
    
    h->http = http_create(url, method);
    for (int i=0; i<m_headers.size(); i++) {
        pair<string, string>& header = m_headers[i];
        http_set_header(h->http, header.first.c_str(), header.second.c_str());
    }
    http_set_body(h->http, body, length);
    h->http_callback = callback;
    
    ((ThreadPool*)m_thread_pool)->add(thread_http, h);
}

void HttpSession::download(const char* url, const char* path, DownCallback* callback){
    mc_down_t* h = bs_new(mc_down);
    
    h->http = http_create(url, "GET");
    for (int i=0; i<m_headers.size(); i++) {
        pair<string, string>& header = m_headers[i];
        http_set_header(h->http, header.first.c_str(), header.second.c_str());
    }
    h->path = path;
    h->down_callback = callback;
    
    ((ThreadPool*)m_thread_pool)->add(thread_down, h);
}

void HttpSession::addHttpHeader(const char* key, const char* value){
    m_headers.push_back(pair<string, string>(key, value));
}

#pragma --mark "http回调及线程"

void* thread_http(void* para){
    mc_http_t* http = (mc_http_t*)para;
    http_res_t* res = http_perform(http->http);
    
    if (http->http_callback!=NULL) {
        if (res->response_code == HTTP_OK){
            http->http_callback->done(HTTP_OK, BS_SUCCESS, res->body);
        }
        // 表示连接有问题
        else if (res->response_code<0) {
            http->http_callback->done(HTTP_INVALID, res->response_code, "http connect error!");
        }
        // 表示http错误
        else if(res->response_code!=HTTP_OK){
            http->http_callback->done(res->response_code, BS_SUCCESS, res->body);
        }
    }
    
    // http->http会在http_perform中被删除
    bs_delete(http);
    bs_delete(res);

    return NULL;
}

void* thread_down(void* para){
    mc_down_t* down = (mc_down_t*)para;
    state_t st = http_download(down->http, down->path.c_str());
    
    if (down->down_callback!=NULL) {
        // 表示连接有问题
        if (st<0) {
            down->down_callback->done(HTTP_INVALID, st, "http connect error!");
        }
        else if(st>0){
            down->down_callback->done(st, BS_SUCCESS, down->path.c_str());
        }
        else{
            down->down_callback->done(HTTP_OK, BS_SUCCESS, down->path.c_str());
        }
    }
    // http->http会在http_perform中被删除
    bs_delete(down);
    
    return NULL;
}
