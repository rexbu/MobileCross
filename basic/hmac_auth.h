//
//  McHmacAuth.h
//  MobileCross
//
//  Created by super on 2017/7/31.
//  Copyright © 2017年 Rex. All rights reserved.
//

#ifndef McHmacAuth_h
#define McHmacAuth_h

#include <stdio.h>

#define MC_AUTH_OUPUT_SIZE 256

namespace mc {
    int mc_base64_encode(const unsigned char *in, int inLen, char *out);
    void mc_auth(char* signstr, char* access_key_id, char* access_key_secret, char auth[MC_AUTH_OUPUT_SIZE]);
};

#endif /* McHmacAuth_h */
