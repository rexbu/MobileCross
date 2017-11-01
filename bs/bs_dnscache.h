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

struct in_addr* dns_cache_lookup(const char *hostname);
void dns_cache_store(struct in_addr in_addr_ip, const char *hostname);
void dns_cache_remove(const char *hostname);
void dns_cache_clear();

#endif /* bs_dnscache_h */
