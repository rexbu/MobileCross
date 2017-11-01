/**
 * file :	bs_url.c
 * author :	bushaofeng
 * create :	2014-10-06 23:05
 * func : 
 * history:
 */

#include "bs_url.h"
#include "bs_socket.h"
#include "bs_dnscache.h"
#include "bs_log.h"

void* url_init(void* p){
    url_t* url = (url_t*)p;
    
    data_init(&url->url);
    data_init(&url->domain);
    data_init(&url->host);
    data_init(&url->path);
    data_init(&url->protocal);
    data_init(&url->query);
    return url;
}
void url_destroy(void* p){
    url_t* url = (url_t*)p;
    
    data_destroy(&url->url);
    data_destroy(&url->domain);
    data_destroy(&url->host);
    data_destroy(&url->path);
    data_destroy(&url->protocal);
    data_destroy(&url->query);
}

void* http_init(void* p){
    http_t* http = (http_t*)p;
    
    url_init(&http->url);
    data_init(&http->req);
    
    return http;
}

void http_destroy(void* p){
    http_t* http = (http_t*)p;
    
    url_destroy(&http->url);
    data_destroy(&http->req);
}

state_t url_parse(url_t* url, const char* url_str){
    int     i;
    char    port[8] = {0};

    if(url==NULL || url_str==NULL){
        return BS_INVALID;
    }

    char* start;
    char* end;

    url->url.set(&url->url, url_str, (uint32_t)strlen(url_str));
    start = (char*)url_str;

    // protocal
    end = strstr(start, "://");
    if(end != NULL){
        url->protocal.set(&url->protocal, start, (uint32_t)(end-start));
        start = end+3;
    }

    // host/port
    end = strchr(start, '/');
    if(end != NULL){
        url->domain.set(&url->domain, start, (uint32_t)(end-start));
        url->host.set(&url->host, start, (uint32_t)(end-start));
        url->port = 80;
        for(i=0; i<end-start; i++){
            if (start[i]==':'){
                url->domain.set(&url->domain, start, i);
                bs_memcpy(port, sizeof(port)-1, start+i+1, (uint32_t)(end-start-i-1));
                url->port = atoi(port);
                break;
            }
        }
        start = end;
    }

    url->path.set(&url->path, start, (uint32_t)strlen(url_str)-(uint32_t)(start - url_str));
    
    // path
    start = strchr(start, '?');
    if (start==NULL){
        return BS_SUCCESS;
    }
    else {
        start += 1;
        url->query.set(&url->query, start, (uint32_t)strlen(url_str)-(uint32_t)(start - url_str));
    }
    
    return BS_SUCCESS;
}

http_t* http_create(const char* url, const char* method){
    char    buffer[URL_SIZE];
    
    http_t* http = bs_new(http);
    url_parse(&http->url, url);
    snprintf(buffer, URL_SIZE, "%s %s HTTP/1.1\r\nAccept: */*\r\nHost: %s\r\n", method, http->url.path.mem, http->url.host.mem);
    data_set(&http->req, buffer, (uint32_t)strlen(buffer));
    
    return http;
}

void http_set_header(http_t* http, const char* key, const char* value){
    char    buffer[URL_SIZE];
    snprintf(buffer, URL_SIZE, "%s: %s\r\n", key, value);
    data_append(&http->req, buffer, (uint32_t)strlen(buffer));
}

void http_add_newline(http_t* http){
    char* newline = "\r\n";
    data_append(&http->req, newline, (uint32_t)strlen(newline));
}

/// 设置http body
void http_set_body(http_t* http, const char* body, uint32_t body_size){
    char buffer[URL_SIZE];
    
    snprintf(buffer, URL_SIZE, "Content-Length: %d\r\nConnection: Close\r\n\r\n", body_size);
    data_append(&http->req, buffer, (uint32_t)strlen(buffer));
    if (body!=NULL && body_size!=0) {
        http->body = http->req.mem + http->req.len;
        data_append(&http->req, body, body_size);
        http->body_size = body_size;
    }
}

http_res_header parse_header(const char *response)
{
    /*获取响应头的信息*/
    struct http_res_header resp;
    
    char *header = strstr(response, "HTTP/1.1");
    //获取http状态代码
    if (header)
        sscanf(header, "%*s %hd", &resp.status_code);
    
    //获取返回文档类型
    header = strstr(response, "Content-Type:");
    if (header)
        sscanf(header, "%*s %s", resp.content_type);
    
    //获取返回文档长度
    header = strstr(response, "Content-Length:");
    if (header)
        sscanf(header, "%*s %lld", &resp.content_length);
                
    return resp;
}

state_t http_timeout(int sock, http_t* http) {
    struct timeval timeout;
    fd_set r;
    FD_ZERO(&r);
    FD_SET(sock,&r);
    timeout.tv_sec = http->time_out;
    timeout.tv_usec = 0;
    int retval = select(sock+1,NULL,&r,NULL,&timeout);
    return retval;
}

http_res_t* http_perform(http_t* http) {
    int sock;
    char buf[1024];
    int len = 0;
    http_res_t *res = bs_new(http_res);
    sock = socket_tcp(BS_TRUE);
    res->response_code = connect_socket(sock, http);
    if (res->response_code == BS_SUCCESS) {
        socket_block(sock);
        if (write(sock, http->req.mem, http->req.len) <= 0) {
            res->response_code = BS_SENDERR;
        } else {
            while( (len = (int)read(sock, buf, sizeof(buf))) > 0){
                data_append(&res->response, buf, len);
            }
            
            http_response_parse(res);
        }
    }
  
    close(sock);
    bs_delete(http);
    return res;
}

http_res_t* http_download(http_t* http, const char* path, void* download, http_progress progressCallback){
    int sock;
    char buf[1024];
    uint64_t len = 0;
    FILE *file;
    uint64_t hasrecieve = 0;
    http_res_t *res = bs_new(http_res);

    sock = socket_tcp(BS_TRUE);
    res->response_code = connect_socket(sock, http);
    if (res->response_code == BS_SUCCESS) {
        socket_block(sock);
        file = fopen(path, "wb+");
        if(file == NULL){
            res->response_code = BS_CREATERR;
        } else {
            if(write(sock, http->req.mem, http->req.len) <= 0){
                res->response_code = BS_SENDERR;
            } else {
                int flag = 0;
                // http response header解析
                while ((len = read(sock, buf, 1)) != 0) {
                    data_append(&res->response, buf, (uint32_t)len);
                    if (buf[0] == '\r' || buf[0] == '\n') {
                        flag ++;
                    } else {
                        flag = 0;
                    }
                    
                    if (flag == 4)
                        break;
                }
                
                // 构建请求成功后的http状态消息
                http_response_parse(res);
                http_res_header res_header = parse_header(res->response.mem);
                
                while (hasrecieve < res_header.content_length) {
                    len = read(sock, buf, sizeof(buf));
                    fwrite(buf, (size_t)len, 1, file);
                    hasrecieve += len;      //更新已经下载的长度
                    progressCallback(len, hasrecieve, res_header.content_length, download); // 回调下载文件的进度值
                }
            }
            
            fclose(file);
        }
    }
    
    close(sock);
    bs_delete(http);
    return res;
}

http_res_t* http_upload(http_t* http, const char* path, void* upload, http_progress progressCallback){
    int sock;
    char buf[1024];
    FILE *file;
    uint64_t hassend = 0;
    uint64_t len = 0;
    uint64_t contentLength = 0;
    http_res_t *res = bs_new(http_res);
    
    sock = socket_tcp(BS_TRUE);
    res->response_code = connect_socket(sock, http);
    if (res->response_code == BS_SUCCESS) {
        socket_block(sock);
        file = fopen(path, "rb");
        if(file == NULL){
            res->response_code = BS_CREATERR;
        } else {
            // 获取文件的大小
            fseek (file , 0 , SEEK_END);
            contentLength = ftell (file);
            rewind (file);
            // 上传http头
            if(write(sock, http->req.mem, http->req.len)<=0){
                res->response_code = BS_SENDERR;
            } else {
                // 设置socket的缓冲区
                int sock_buf_size = 10000;
                setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&sock_buf_size, sizeof(sock_buf_size));
                
                //上传http文件
                while ((len = fread(buf, 1, sizeof(buf), file)) > 0) {
                    if (write(sock, buf, (size_t)len) <= -1) {
                        res->response_code = BS_SENDERR;
                    } else {
                        hassend += len;
                        if (hassend / len % 10 == 1 || hassend >= contentLength) {
                            progressCallback(len, hassend, contentLength, upload); // 回调上传文件的进度值
                        }
                    }
                }
                
                uint64_t read_length = 0;
                if ((read_length = read(sock, buf, sizeof(buf))) > 0) {
                    data_append(&res->response, buf, (uint32_t)read_length);
                }
                
                if (read_length == -1) {
                    res->response_code = BS_SENDERR;
                } else {
                    http_response_parse(res);
                }
            }
            
            fclose(file);
        }
    }
    
    close(sock);
    bs_delete(http);
    return res;
}

http_res_t* http_post_data(http_t* http, uint8_t* data, void* upload, http_progress progressCallback) {
    int         sock;
    char        buf[1024];
    uint64_t    hassend = 0;
    uint32_t    bufferSize = 1024;
    uint64_t    len = 0;
    http_res_t* res = bs_new(http_res);
    
    sock = socket_tcp(BS_TRUE);
    res->response_code = connect_socket(sock, http);
    if (res->response_code == BS_SUCCESS) {
        socket_block(sock);
        // 上传http头
        if (write(sock, http->req.mem, http->req.len)<=0){
            res->response_code = BS_SENDERR;
        } else {
            //上传二进制数据
            size_t sock_buf_size = 10000;
            setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&sock_buf_size, sizeof(sock_buf_size));
            uint64_t totalSize = http->body_size;
            len = bufferSize;
            while (hassend < totalSize) {
                memset(buf, 0, bufferSize);
                uint64_t remainSize = totalSize - hassend;
                len = remainSize > bufferSize ? bufferSize : remainSize;
                memcpy(buf, data+hassend, len);
                if (write(sock, buf, (size_t)len) <= -1) {
                    close(sock);
                    bs_delete(http);
                    res->response_code = BS_SENDERR;
                    return res;
                } else {
                    hassend += len;
                    if (hassend / len % 10 == 1 || hassend >= totalSize) {
                        progressCallback(len, hassend, totalSize, upload);
                    }
                }
            }
            
            uint64_t read_length = 0;
            while((read_length = (int)read(sock, buf, sizeof(buf))) > 0) {
                data_append(&res->response, buf, (uint32_t)read_length);
                if (read_length < bufferSize) {
                    break;
                }
            }
            
            if (read_length == -1) {
                res->response_code = BS_SENDERR;
            } else {
                http_response_parse(res);
            }
        }
    }
    
    close(sock);
    bs_delete(http);
    return res;
}

void* http_res_init(void * p){
    http_res_t* res = (http_res_t*)p;
    data_init(&res->response);
    res->response_code = 0;
    res->body = NULL;
    res->body_size = 0;
    return res;
}

void http_res_destroy(void* p){
    http_res_t* res = (http_res_t*)p;
    data_destroy(&res->response);
}

state_t connect_socket(int sock, http_t *http) {
    bs_sock_ignore_sigpipe(sock);
    
    int err = bs_sock_connect(sock, http->url.domain.mem, http->url.port);
    if (err == -1 && errno != EINPROGRESS) {
        return BS_CONNERR;
    }
    
    int retval = http_timeout(sock, http);
    if(retval==-1 || retval == 0) {
        return retval == -1 ? BS_CONNERR : BS_TIMEOUT;
    }
    
    return BS_SUCCESS;
}

state_t http_response_parse(http_res_t* res){
    if (res == NULL) {
        return BS_PARANULL;
    }
    
    http_res_header res_header = parse_header(res->response.mem);
    sscanf(res->response.mem, "HTTP/1.1 %d", &res->response_code);
    res->body = bs_strrstr(res->response.mem, "\r\n") + 2;
    res->body_size = res->response.len - (uint32_t)(res->body-res->response.mem);
    
    if (res_header.status_code == HTTP_NOTFOUND) {
        dns_cache_clear();
    }
    
    return BS_SUCCESS;
}

void url_print(url_t* url){
    fprintf(stdout, "protocal[%s] host[%s] domain[%s] port[%d] path[%s] query[%s]\n", url->protocal.mem, url->host.mem, url->domain.mem, url->port, url->path.mem, url->query.mem);
}
