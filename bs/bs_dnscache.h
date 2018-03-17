//
//  bs_dnscache.h
//  MobileCross
//
//  Created by super on 2017/8/1.
//  Copyright © 2017年 Rex. All rights reserved.
//

#ifndef bs_dnscache_h
#define bs_dnscache_h

#include "bs_type.h"

typedef struct bs_dns {
    struct bs_dns *next;
    struct in_addr *in_addr_ip;
    struct in6_addr *in_addr_ip6;
    char *hostname;
} bs_dns;

bs_dns* dns_cache_lookup(const char *hostname);
void dns_cache_store(struct in_addr in_addr_ip, const char *hostname);
void dns_cache_ipv6_store(struct in6_addr in_addr_ip, const char *hostname);
void dns_cache_remove(const char *hostname);
void dns_cache_clear();

#endif /* bs_dnscache_h */
