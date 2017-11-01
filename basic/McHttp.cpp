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
#include "SharedPreferences.h"
#include "McFile.h"
#include "McZip.h"
#include <iterator>
#include "JSON.h"

using namespace mc;

#pragma --mark "http结构体封装"
typedef struct {
    object_t base;
    http_t *http;
    HttpCallback *http_callback;
} mc_http_t;

extern "C" void *mc_http_init(void *h) {
    mc_http_t *http = (mc_http_t *) h;
    http->http = NULL;
    http->http_callback = NULL;
    return http;
}

extern "C" void mc_http_destroy(void *h) {
}

typedef struct {
    object_t base;
    http_t *http;
    char path[1024];
    HttpFileCallback *file_callback;
} mc_http_file_t;

extern "C" void *mc_http_file_init(void *p) {
    mc_http_file_t *http_file = (mc_http_file_t *) p;
    http_file->http = NULL;
    http_file->file_callback = NULL;
    return http_file;
}
extern "C" void mc_http_file_destroy(void *p) {

}

typedef struct {
    object_t base;
    http_t *http;
    uint8_t *data;
    HttpFileCallback *file_callback;
} mc_http_data_t;

extern "C" void *mc_http_data_init(void *h) {
    mc_http_data_t *http = (mc_http_data_t *) h;
    http->http = NULL;
    http->data = NULL;
    http->file_callback = NULL;
    return http;
}
extern "C" void mc_http_data_set(mc_http_data_t *h, void *data, int size) {
    h->data = (uint8_t *) malloc(size);
    memcpy(h->data, data, size);
}

extern "C" void mc_http_data_destroy(void *h) {
    mc_http_data_t *http = (mc_http_data_t *) h;
    if (http->data != NULL) {
        free(http->data);
    }
}

#pragma --mark "HttpSession 定义"

// 数据获取函数
void *thread_http(void *para);

void *thread_down(void *para);

void *thread_up_file(void *para);

void *thread_post_data(void *para);

void thread_http_response(http_res_t *res, HttpSession *session);

uint32_t HttpSession::http_timeout = 20;
bool HttpSession::http_show_log = false;

HttpSession::HttpSession(void *threadpool) {
    if (threadpool == NULL) {
        m_thread_pool = ThreadPool::shareInstance();
    } else {
        m_thread_pool = threadpool;
    }
}

HttpSession::~HttpSession() {
}

void HttpSession::setTimeout(uint32_t timeout) {
    http_timeout = timeout;
}

void HttpSession::showLog(bool show) {
    http_show_log = show;
}

void HttpSession::get(const char *url, HttpCallback *callback) {
    http(url, "GET", NULL, 0, callback);
}

void HttpSession::post(const char *url, const char *body, uint32_t length, HttpCallback *callback) {
    http(url, "POST", body, length, callback);
}

void HttpSession::put(const char *url, const char *body, uint32_t length, HttpCallback *callback) {
    http(url, "PUT", body, length, callback);
}

void HttpSession::del(const char *url, HttpCallback *callback) {
    http(url, "DELETE", NULL, 0, callback);
}

void HttpSession::http(const char *url, const char *method, const char *body, uint32_t length,
                       HttpCallback *callback) {
    mc_http_t *h = bs_new(mc_http);

    h->http = http_create(url, method);
    h->http->time_out = http_timeout;
    for (map<string, string>::iterator iter = m_headers.begin(); iter != m_headers.end(); iter++) {
        http_set_header(h->http, iter->first.c_str(), iter->second.c_str());
    }
    http_set_body(h->http, body, length);
    h->http_callback = callback;

    void **para = (void **) malloc(sizeof(void *) * 2);
    para[0] = h;
    para[1] = this;
    ((ThreadPool *) m_thread_pool)->add(thread_http, para);

#if DEBUG
    if (http_show_log) {
        info_log("%s", h->http->req.mem);
    }
#else
#endif
}

void HttpSession::download(const char *url, const char *path, HttpFileCallback *callback) {
    mc_http_file_t *h = bs_new(mc_http_file);

    h->http = http_create(url, "GET");
    h->http->time_out = http_timeout;
    for (map<string, string>::iterator iter = m_headers.begin(); iter != m_headers.end(); iter++) {
        http_set_header(h->http, iter->first.c_str(), iter->second.c_str());
    }

    http_add_newline(h->http);
    snprintf(h->path, sizeof(h->path), "%s", path);
    h->file_callback = callback;

    pthread_t download_thread;
    pthread_create(&download_thread, NULL, thread_down, h);
}

void HttpSession::upload(const char *url, const char *path, vector<pair<string, string> > auths,
                         HttpFileCallback *callback) {
    mc_http_file_t *h = bs_new(mc_http_file);
    h->http = http_create(url, "PUT");
    h->http->time_out = http_timeout;

    for (int i = 0; i < auths.size(); i++) {
        pair<string, string> &auth = auths[i];
        http_set_header(h->http, auth.first.c_str(), auth.second.c_str());
    }

    http_add_newline(h->http);
    snprintf(h->path, sizeof(h->path), "%s", path);
    h->file_callback = callback;

    pthread_t upload_thread;
    pthread_create(&upload_thread, NULL, thread_up_file, h);
}

void
HttpSession::upload(const char *url, uint8_t *data, int size, vector<pair<string, string> > auths,
                    HttpFileCallback *callback) {
    mc_http_data_t *h = bs_new(mc_http_data);
    h->http = http_create(url, "PUT");
    h->http->time_out = http_timeout;

    for (int i = 0; i < auths.size(); i++) {
        pair<string, string> &auth = auths[i];
        http_set_header(h->http, auth.first.c_str(), auth.second.c_str());
    }

    http_add_newline(h->http);
    mc_http_data_set(h, data, size);
    h->http->body_size = (uint32_t) size;
    h->file_callback = callback;

    void **para = (void **) malloc(sizeof(void *) * 2);
    para[0] = h;
    para[1] = this;

    pthread_t post_data_thread;
    pthread_create(&post_data_thread, NULL, thread_post_data, para);
}

void HttpSession::postData(const char *url, uint8_t *data, int size, HttpFileCallback *callback) {
    mc_http_data_t *h = bs_new(mc_http_data);
    h->http = http_create(url, "POST");
    h->http->time_out = http_timeout;
    for (map<string, string>::iterator iter = m_headers.begin(); iter != m_headers.end(); iter++) {
        http_set_header(h->http, iter->first.c_str(), iter->second.c_str());
    }

    char contentLength[10] = "\0";
    snprintf(contentLength, 10, "%d", size);
    http_set_header(h->http, "Content-Length", contentLength);

    http_add_newline(h->http);
    mc_http_data_set(h, data, size);
    h->http->body_size = (uint32_t) size;
    h->file_callback = callback;

    void **para = (void **) malloc(sizeof(void *) * 2);
    para[0] = h;
    para[1] = this;

    pthread_t post_data_thread;
    pthread_create(&post_data_thread, NULL, thread_post_data, para);

#if DEBUG
    if (http_show_log) {
        info_log("%s", h->http->req.mem);
    }
#else
#endif
}

void HttpSession::addHttpHeader(const char *key, const char *value) {
    m_headers[string(key)] = string(value);
}

#pragma --mark "cookie管理"

const char *HttpSession::cookie() {
    return m_headers["Cookie"].c_str();
}

void HttpSession::saveCookie(const char *cookie) {
    setCookie(cookie);
    Crypt crypt;
    JSONObject cookieObject = JSONObject("root");
    cookieObject.put("cookie", cookie);
    const char *str = cookieObject.toString();
    int64_t crysize = crypt.encryptByCompress((byte *) str, (uint32_t) strlen(str));

    char path[1024];
    SharedPreferences::path("__CurrentCookie", path, sizeof(path));
    FileManager::write(path, crypt.bytes(), (size_t) crysize);
}

void HttpSession::setCookie(const char *cookie) {
    addHttpHeader("Cookie", cookie);
}

void HttpSession::clearCookie() {
    char path[1024];
    SharedPreferences::path("__CurrentCookie", path, sizeof(path));
    FileManager::remove(path);
    map<string, string>::iterator cookie = m_headers.find("Cookie");
    m_headers.erase(cookie);
}

#pragma --mark "http回调及线程"
void *thread_http(void *para) {
    void **arg = (void **) para;
    mc_http_t *http = (mc_http_t *) arg[0];

    http_res_t *res = http_perform(http->http);
    if (http->http_callback != NULL) {
        thread_http_response(res, (HttpSession *) arg[1]);
        http->http_callback->done(res->response_code, res->response_code, res->body);
    }

    // http->http会在http_perform中被删除
    free(arg);
    bs_delete(http);
    bs_delete(res);

    return NULL;
}

void thread_http_file_progress(uint64_t writen, uint64_t total_writen, uint64_t total_expect_write,
                               void *mc_http) {
    mc_http_file_t *http_file = (mc_http_file_t *) mc_http;
    http_file->file_callback->progress(writen, total_writen, total_expect_write);
}

void thread_http_Data_progress(uint64_t writen, uint64_t total_writen, uint64_t total_expect_write,
                               void *mc_http) {
    mc_http_data_t *http_data = (mc_http_data_t *) mc_http;
    http_data->file_callback->progress(writen, total_writen, total_expect_write);
}

void *thread_down(void *para) {
    mc_http_file_t *down = (mc_http_file_t *) para;
    http_res_t *res = http_download(down->http, down->path, (void *) down, &thread_http_file_progress);

    if (down->file_callback != NULL) {
        // 表示连接有问题
        thread_http_response(res, NULL);
        down->file_callback->done(res->response_code, res->response_code, res->body);
    }
    // http->http会在http_perform中被删除
    bs_delete(down);

    return NULL;
}

void *thread_up_file(void *para) {
    mc_http_file_t *up = (mc_http_file_t *) para;
    http_res_t *res = http_upload(up->http, up->path, (void *) up, &thread_http_file_progress);

    if (up->file_callback != NULL) {
        // 表示连接有问题
        thread_http_response(res, NULL);
        up->file_callback->done(res->response_code, res->response_code, res->body);
    }
    // http->http会在http_perform中被删除
    bs_delete(up);

    return NULL;
}

void *thread_post_data(void *para) {
    void **arg = (void **) para;
    mc_http_data_t *up = (mc_http_data_t *) arg[0];
    http_res_t *res = http_post_data(up->http, up->data, (void *) up, &thread_http_Data_progress);

    if (up->file_callback != NULL) {
        // 表示连接有问题
        thread_http_response(res, (HttpSession *) arg[1]);
        up->file_callback->done(res->response_code, res->response_code, res->body);
    }
    // http->http会在http_perform中被删除
    free(arg);
    bs_delete(up);
    bs_delete(res);
    return NULL;
}

void thread_http_response(http_res_t *res, HttpSession *session) {
    if (res->response_code < 0) {
        const char *error = "http connect error!";
        if (res->response_code == BS_TIMEOUT) {
            error = "http time-out";
        }
        
        res->body = (char *)error;
    } else {
        if (session != NULL) {
            res->cookie = bs_strrstr(res->response.mem, "\r\nSet-Cookie");
            if (res->cookie != NULL) {
                sscanf(res->cookie, "\r\nSet-Cookie: %[^;]", res->cookie);
                session->saveCookie(res->cookie);
            }
        }
    }
    
#if DEBUG
    if (session->http_show_log) {
        if (res->response_code == HTTP_OK) {
            JSONObject response(res->body);
            info_log("%s", response.toString());
        } else {
            info_log("%s", res->response.mem);
        }
    }
#else
#endif
}
