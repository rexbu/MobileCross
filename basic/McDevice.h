/**
 * file :	McDevice.h
 * author :	Rex
 * create :	2016-10-26 15:28
 * func :   设备及系统相关
 * history:
 */

#ifndef	__MCDEVICE_H_
#define	__MCDEVICE_H_

#include <vector>
#include <map>
#include "bs.h"
#ifdef __ANDROID__
#include "jni.h"
#else
#endif

// uid为24个字节的字符创
#define BS_UNIQUE_ID_LENGTH   30
#define BS_UNIQUE_ID_EFFECT_LENGTH   24

namespace mc {
    const char* device_id();
    const char* bundle_id();
    const char* package_name();
    const char* device_version();
    const char* system_version();
    
#ifdef __ANDROID__
    void mc_set_device(JNIEnv* env,jobject thiz,jobject context);
#else
#endif

    int device_index();

    void guid(char objectid[BS_UNIQUE_ID_LENGTH]);
}

#endif
