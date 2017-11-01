//
//  bs_dnscache.c
//  MobileCross
//
//  Created by super on 2017/8/1.
//  Copyright © 2017年 Rex. All rights reserved.
//

#include "bs_dnscache.h"
#include <string.h>

typedef struct bs_dns{
    struct bs_dns *next;
    struct in_addr in_addr_ip;
    char *hostname;
}bs_dns;

typedef bs_dns *bs_dns_pr;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// 只缓存5个ip
#define BS_HOST_CACHE_COUNT 5
// 根节点不存储dns信息
static bs_dns_pr m_root;

bool_t dns_cache_empty() {
    if (m_root == NULL) {
        return BS_TRUE;
    }
    return BS_FALSE;
}

void dns_root_create() {
    m_root = (bs_dns_pr)malloc(sizeof(bs_dns));
    m_root->hostname = NULL;
    m_root->next = NULL;
}

struct in_addr* dns_cache_lookup(const char *hostname) {
    if (pthread_mutex_lock(&mutex) != 0){
        fprintf(stdout, "lock error!\n");
    }
    if (dns_cache_empty()) {
        pthread_mutex_unlock(&mutex);
        return NULL;
    }
    
    bs_dns_pr dns = m_root->next;
    while (dns) {
        if (strcmp(dns->hostname, hostname) == 0) {
            pthread_mutex_unlock(&mutex);
            return &dns->in_addr_ip;
        }
        dns = dns->next;
    }

    pthread_mutex_unlock(&mutex);
    return NULL;
}

void dns_cache_store(struct in_addr in_addr_ip, const char *hostname) {
    if (pthread_mutex_lock(&mutex) != 0){
        fprintf(stdout, "lock error!\n");
    }
    if (dns_cache_empty()) {
        dns_root_create();
    }
    
    int cacheCount = 0;
    bs_dns_pr dns_pre = m_root;
    bs_dns_pr dns_next = dns_pre->next;
    while (dns_next) {
        cacheCount++;
        dns_pre = dns_next;
        dns_next = dns_next->next;
        if (strcmp(hostname, dns_pre->hostname) == 0) {
            pthread_mutex_unlock(&mutex);
            return;
        }
    }
    
    if (cacheCount < BS_HOST_CACHE_COUNT) {
        // 存储dns信息时候需要加锁，处理线程安全
        bs_dns_pr node = (bs_dns_pr)malloc(sizeof(bs_dns));
        dns_pre->next = node;
        node->in_addr_ip = in_addr_ip;
        node->next = NULL;
        
        int len = (int)strlen(hostname) + 1;
        if (!(node->hostname = (char*)malloc(sizeof(char) * len))) {
            pthread_mutex_unlock(&mutex);
            return;
        }
        memcpy(node->hostname, hostname, len);
        printf("");
    }
  
    // 解锁
    pthread_mutex_unlock(&mutex);
}

void dns_cache_remove(const char *hostname) {
    if (pthread_mutex_lock(&mutex) != 0){
        fprintf(stdout, "lock error!\n");
    }
    if (dns_cache_empty()) {
        pthread_mutex_unlock(&mutex);
        return;
    }
    
    bs_dns_pr dns_pre = m_root;
    bs_dns_pr dns_next = m_root->next;
    while (dns_next) {
        if (strcmp(dns_next->hostname, hostname)) {
            dns_pre = dns_next->next;
            free(dns_next->hostname);
            free(dns_next);
            pthread_mutex_unlock(&mutex);
            return;
        }
        
        dns_pre = dns_next;
        dns_next = dns_next->next;
    }
    
    pthread_mutex_unlock(&mutex);
}

void dns_cache_clear() {
    if (pthread_mutex_lock(&mutex) != 0){
        fprintf(stdout, "lock error!\n");
    }
    if (dns_cache_empty()) {
        pthread_mutex_unlock(&mutex);
        return;
    }
    
    bs_dns_pr dns = m_root;
    while (dns) {
        bs_dns_pr node = dns;
        dns = node->next;
        free(node->hostname);
        free(node);
    }
    
    m_root = NULL;
    pthread_mutex_unlock(&mutex);
}

