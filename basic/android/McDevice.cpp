/**
 * file :	McDevice.cpp
 * author :	Rex
 * create :	2016-11-25 16:26
 * func : 
 * history:
 */
#include <jni.h>
#include <iostream>
#include <string>
#include "bs.h"
#include "../McDevice.h"
#include "../McFile.h"

using namespace std;

static string g_device_id;
static string g_phone;
static string g_sim;
static string g_packagename;
static string g_device;
static string g_system;
static string g_manufacturer;

void mc::mc_set_device(JNIEnv *env, jobject thiz, jobject context) {
    jclass deviceClass = env->FindClass("com/rex/utils/DeviceUtil");
    const char *s = NULL;

    jmethodID method = env->GetStaticMethodID(deviceClass, "deviceId",
                                              "(Landroid/content/Context;)Ljava/lang/String;");
    jstring deviceid = (jstring) env->CallStaticObjectMethod(deviceClass, method, context);
    if (deviceid != NULL) {
        s = env->GetStringUTFChars(deviceid, NULL);
        debug_log("DeviceId: %s", s);
        g_device_id = s;
        env->ReleaseStringUTFChars(deviceid, s);
    }

    method = env->GetStaticMethodID(deviceClass, "getLine1Number",
                                    "(Landroid/content/Context;)Ljava/lang/String;");
    deviceid = (jstring) env->CallStaticObjectMethod(deviceClass, method, context);
    if (deviceid != NULL) {
        s = env->GetStringUTFChars(deviceid, NULL);
        debug_log("Line1Number: %s", s);
        g_phone = s;
        env->ReleaseStringUTFChars(deviceid, s);
    }

    method = env->GetStaticMethodID(deviceClass, "getSimSerialNumber",
                                    "(Landroid/content/Context;)Ljava/lang/String;");
    deviceid = (jstring) env->CallStaticObjectMethod(deviceClass, method, context);
    if (deviceid != NULL) {
        s = env->GetStringUTFChars(deviceid, NULL);
        debug_log("SimSerialNumber: %s", s);
        g_sim = s;
        env->ReleaseStringUTFChars(deviceid, s);
    }

    char package[256] = {0};
    FILE *fp = fopen("/proc/self/cmdline", "r");

    fread(package, sizeof(package), 1, fp);
    fclose(fp);
    debug_log("PackageName: %s", package);
    g_packagename = package;

    method = env->GetStaticMethodID(deviceClass, "getDeviceModel", "()Ljava/lang/String;");
    deviceid = (jstring) env->CallStaticObjectMethod(deviceClass, method, context);
    if (deviceid != NULL) {
        s = env->GetStringUTFChars(deviceid, NULL);
        debug_log("DeviceModel: %s", s);
        g_device = s;
        env->ReleaseStringUTFChars(deviceid, s);
    }

    method = env->GetStaticMethodID(deviceClass, "getSystemVersion", "()Ljava/lang/String;");
    deviceid = (jstring) env->CallStaticObjectMethod(deviceClass, method, context);
    if (deviceid != NULL) {
        s = env->GetStringUTFChars(deviceid, NULL);
        debug_log("SystemVersion: %s", s);
        g_system = s;
        env->ReleaseStringUTFChars(deviceid, s);
    }

    method = env->GetStaticMethodID(deviceClass, "getManufacturer", "()Ljava/lang/String;");
    deviceid = (jstring) env->CallStaticObjectMethod(deviceClass, method, context);
    if (deviceid != NULL) {
        s = env->GetStringUTFChars(deviceid, NULL);
        debug_log("Manufacturer: %s", s);
        g_manufacturer = s;
        env->ReleaseStringUTFChars(deviceid, s);
    }

    env->DeleteLocalRef(deviceClass);
}

const char *mc::device_id() {
    return g_device_id.c_str();
}

const char *mc::system_version() {
    return g_system.c_str();
}

const char *mc::device_version() {
    return g_device.c_str();
}

int mc::device_index() {
    return 0;
}

const char *mc::bundle_id() {
    return NULL;
}

const char *mc::package_name() {
    return g_packagename.c_str();
}

int uidBaseIncrease = 0;

void mc::guid(char objectid[BS_UNIQUE_ID_LENGTH]) {
    unsigned char uid[12];
    timeval start;
    gettimeofday(&start, NULL);
    if (uidBaseIncrease <= 0) {
        srand((unsigned) time(0));
        uidBaseIncrease = rand();
    } else {
        uidBaseIncrease += 1;
    }
    short a = 1;
    if (((char *) &a)[0] == 1) {
        uid[0] = ((char *) &start.tv_sec)[3];
        uid[1] = ((char *) &start.tv_sec)[2];
        uid[2] = ((char *) &start.tv_sec)[1];
        uid[3] = ((char *) &start.tv_sec)[0];
        uid[7] = ((char *) &start.tv_sec)[1];
        uid[8] = ((char *) &start.tv_sec)[0];
        uid[9] = ((char *) &uidBaseIncrease)[2];
        uid[10] = ((char *) &uidBaseIncrease)[1];
        uid[11] = ((char *) &uidBaseIncrease)[0];
    } else {
        *(uint32_t *) uid = start.tv_sec;
        memcpy(uid + 7, &start.tv_usec, 2);
        memcpy(uid + 9, &uidBaseIncrease, 3);
    }
    //memcpy(uid, &start.tv_sec, 4);

    const char *device = mc::device_id();
    uint32_t crc = bs_crc32(0, (void *) device, (uint32_t) strlen(device));
    memcpy(uid + 4, &crc, 3);
    *(uint16_t *) (uid + 7) = start.tv_usec;

    char id[BS_UNIQUE_ID_LENGTH];
    for (int i = 0; i < 12; i++) {
        sprintf(id + i * 2, "%02x", uid[i]);
    }

    id[BS_UNIQUE_ID_EFFECT_LENGTH] = '\0';
    snprintf(objectid, BS_UNIQUE_ID_LENGTH, "%s", id);
}
