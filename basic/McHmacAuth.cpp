//
//  McHmacAuth.cpp
//  MobileCross
//
//  Created by super on 2017/7/31.
//  Copyright © 2017年 Rex. All rights reserved.
//

#include "McHmacAuth.h"
#include <string.h>
#include "HMAC_SHA1.h"

#define MAX_MESSAGE_LENGTH 4096

int mc::mc_base64_encode(const unsigned char *in, int inLen, char *out)
{
    static const char *ENC =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    char *original_out = out;
    
    while (inLen) {
        // first 6 bits of char 1
        *out++ = ENC[*in >> 2];
        if (!--inLen) {
            // last 2 bits of char 1, 4 bits of 0
            *out++ = ENC[(*in & 0x3) << 4];
            *out++ = '=';
            *out++ = '=';
            break;
        }
        // last 2 bits of char 1, first 4 bits of char 2
        *out++ = ENC[((*in & 0x3) << 4) | (*(in + 1) >> 4)];
        in++;
        if (!--inLen) {
            // last 4 bits of char 2, 2 bits of 0
            *out++ = ENC[(*in & 0xF) << 2];
            *out++ = '=';
            break;
        }
        // last 4 bits of char 2, first 2 bits of char 3
        *out++ = ENC[((*in & 0xF) << 2) | (*(in + 1) >> 6)];
        in++;
        // last 6 bits of char 3
        *out++ = ENC[*in & 0x3F];
        in++, inLen--;
    }
    
    return (out - original_out);
}

void mc::mc_auth(char* signstr, char* access_key_id, char* access_key_secret, char auth[MC_AUTH_OUPUT_SIZE])
{
    int b64Len = 0;
    unsigned char hmac[20];
    char value[MC_AUTH_OUPUT_SIZE];
    char b64[((20 + 1) * 4) / 3];
    
    CHMAC_SHA1 HMAC_SHA1 ;
    HMAC_SHA1.HMAC_SHA1((unsigned char*)signstr, (int)strlen(signstr), (unsigned char*)access_key_secret, (int)strlen(access_key_secret), hmac);
    
    b64Len = mc_base64_encode(hmac, 20, b64);
    strncpy(value, b64, b64Len);
    value[b64Len] = '\0';
    snprintf(auth, MC_AUTH_OUPUT_SIZE, "OSS %s:%s", access_key_id, value);
}
