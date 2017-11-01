/**
 * file :	McDevice.mm
 * author :	Rex
 * create :	2016-10-26 15:29
 * func : 
 * history:
 */

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#include <sys/utsname.h>
#import <CommonCrypto/CommonDigest.h>
#include "McDevice.h"
#import "SecureUDID.h"
#include <sys/sysctl.h>
#include <net/if_dl.h>
#include "McDevice.h"
#include <string.h>

#define KEYCHAIN_SERVICE_NAME       @"visioninsdk.com.visionin.rex"
#define KEYCHAIN_ACCOUNT_UUID       @"keychain_account_uuid.com.visionin.rex"

const char* mc::device_id()
{
    NSString *UUID = [[NSUserDefaults standardUserDefaults] objectForKey:KEYCHAIN_SERVICE_NAME];
    if (!UUID || [UUID isEqualToString:@""]) {
        
        NSString *domain     = KEYCHAIN_SERVICE_NAME;
        NSString *key        = KEYCHAIN_ACCOUNT_UUID;
        NSString *identifier = [SecureUDID UDIDForDomain:[domain stringByAppendingString:@"uuid"] usingKey:[key stringByAppendingString:@"uuid"]];
        //本地没有，创建UUID
        UUID = identifier;
        if (identifier && ![identifier isEqualToString:@""]) {
            [[NSUserDefaults standardUserDefaults] setObject:identifier forKey:KEYCHAIN_SERVICE_NAME];
        }
    }
  
    NSString *format = [UUID stringByReplacingOccurrencesOfString:@"-" withString:@""];
    return [format UTF8String];
}

const char* mc::system_version(){
    NSString* version = [[UIDevice currentDevice] systemVersion];
    return [version UTF8String];
}

const char* mc::device_version()
{
    // 需要#import "sys/utsname.h"
    struct utsname systemInfo;
    uname(&systemInfo);
    NSString *deviceString = [NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding];
    NSString* deviceVersion = nil;
    //iPhone
    if ([deviceString isEqualToString:@"iPhone1,1"])    deviceVersion = @"iPhone 1G";
    if ([deviceString isEqualToString:@"iPhone1,2"])    deviceVersion = @"iPhone 3G";
    if ([deviceString isEqualToString:@"iPhone2,1"])    deviceVersion = @"iPhone 3GS";
    if ([deviceString isEqualToString:@"iPhone3,1"])    deviceVersion = @"iPhone 4";
    if ([deviceString isEqualToString:@"iPhone3,2"])    deviceVersion = @"Verizon iPhone 4";
    if ([deviceString isEqualToString:@"iPhone4,1"])    deviceVersion = @"iPhone 4S";
    if ([deviceString isEqualToString:@"iPhone5,1"])    deviceVersion = @"iPhone 5";
    if ([deviceString isEqualToString:@"iPhone5,2"])    deviceVersion = @"iPhone 5";
    if ([deviceString isEqualToString:@"iPhone5,3"])    deviceVersion = @"iPhone 5C";
    if ([deviceString isEqualToString:@"iPhone5,4"])    deviceVersion = @"iPhone 5C";
    if ([deviceString isEqualToString:@"iPhone6,1"])    deviceVersion = @"iPhone 5S";
    if ([deviceString isEqualToString:@"iPhone6,2"])    deviceVersion = @"iPhone 5S";
    if ([deviceString isEqualToString:@"iPhone7,1"])    deviceVersion = @"iPhone 6 Plus";
    if ([deviceString isEqualToString:@"iPhone7,2"])    deviceVersion = @"iPhone 6";
    if ([deviceString isEqualToString:@"iPhone8,1"])    deviceVersion = @"iPhone 6s";
    if ([deviceString isEqualToString:@"iPhone8,2"])    deviceVersion = @"iPhone 6s Plus";
    if ([deviceString isEqualToString:@"iPhone9,1"])    deviceVersion = @"iPhone 7";
    if ([deviceString isEqualToString:@"iPhone9,2"])    deviceVersion = @"iPhone 7 Plus";
    
    //iPod
    if ([deviceString isEqualToString:@"iPod1,1"]) deviceVersion = @"iPod Touch 1G";
    if ([deviceString isEqualToString:@"iPod2,1"]) deviceVersion = @"iPod Touch 2G";
    if ([deviceString isEqualToString:@"iPod3,1"]) deviceVersion = @"iPod Touch 3G";
    if ([deviceString isEqualToString:@"iPod4,1"]) deviceVersion = @"iPod Touch 4G";
    if ([deviceString isEqualToString:@"iPod5,1"]) deviceVersion = @"iPod Touch 5G";
    
    //iPad
    if ([deviceString isEqualToString:@"iPad1,1"])      deviceVersion = @"iPad";
    if ([deviceString isEqualToString:@"iPad2,1"])      deviceVersion = @"iPad 2 (WiFi)";
    if ([deviceString isEqualToString:@"iPad2,2"])      deviceVersion = @"iPad 2 (GSM)";
    if ([deviceString isEqualToString:@"iPad2,3"])      deviceVersion = @"iPad 2 (CDMA)";
    if ([deviceString isEqualToString:@"iPad2,4"])      deviceVersion = @"iPad 2 (32nm)";
    if ([deviceString isEqualToString:@"iPad2,5"])      deviceVersion = @"iPad mini (WiFi)";
    if ([deviceString isEqualToString:@"iPad2,6"])      deviceVersion = @"iPad mini (GSM)";
    if ([deviceString isEqualToString:@"iPad2,7"])      deviceVersion = @"iPad mini (CDMA)";
    
    if ([deviceString isEqualToString:@"iPad3,1"])      deviceVersion = @"iPad 3(WiFi)";
    if ([deviceString isEqualToString:@"iPad3,2"])      deviceVersion = @"iPad 3(CDMA)";
    if ([deviceString isEqualToString:@"iPad3,3"])      deviceVersion = @"iPad 3(4G)";
    if ([deviceString isEqualToString:@"iPad3,4"])      deviceVersion = @"iPad 4 (WiFi)";
    if ([deviceString isEqualToString:@"iPad3,5"])      deviceVersion = @"iPad 4 (4G)";
    if ([deviceString isEqualToString:@"iPad3,6"])      deviceVersion = @"iPad 4 (CDMA)";
    
    if ([deviceString isEqualToString:@"iPad4,1"])      deviceVersion = @"iPad Air";
    if ([deviceString isEqualToString:@"iPad4,2"])      deviceVersion = @"iPad Air";
    if ([deviceString isEqualToString:@"iPad4,3"])      deviceVersion = @"iPad Air";
    if ([deviceString isEqualToString:@"iPad5,3"])      deviceVersion = @"iPad Air 2";
    if ([deviceString isEqualToString:@"iPad5,4"])      deviceVersion = @"iPad Air 2";
    if ([deviceString isEqualToString:@"i386"])         deviceVersion = @"Simulator";
    if ([deviceString isEqualToString:@"x86_64"])       deviceVersion = @"Simulator";
    
    if ([deviceString isEqualToString:@"iPad4,4"]||[deviceString isEqualToString:@"iPad4,5"]||[deviceString isEqualToString:@"iPad4,6"]) deviceVersion = @"iPad mini 2";
    if ([deviceString isEqualToString:@"iPad4,7"]||[deviceString isEqualToString:@"iPad4,8"]||[deviceString isEqualToString:@"iPad4,9"])  deviceVersion = @"iPad mini 3";
    
    if (deviceVersion == nil) {
        deviceVersion = deviceString;
    }
    
    return [deviceVersion UTF8String];
}

int mc::device_index(){
    struct utsname systemInfo;
    uname(&systemInfo);
    NSString *deviceString = [NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding];
    
    //iPhone
    if ([deviceString isEqualToString:@"iPhone1,1"])    return 11; // @"iPhone 1G";
    if ([deviceString isEqualToString:@"iPhone1,2"])    return 12; // @"iPhone 3G";
    if ([deviceString isEqualToString:@"iPhone2,1"])    return 21; // @"iPhone 3GS";
    if ([deviceString isEqualToString:@"iPhone3,1"])    return 31; // @"iPhone 4";
    if ([deviceString isEqualToString:@"iPhone3,2"])    return 32; // @"Verizon iPhone 4";
    if ([deviceString isEqualToString:@"iPhone4,1"])    return 41; // @"iPhone 4S";
    if ([deviceString isEqualToString:@"iPhone5,1"])    return 51; // @"iPhone 5";
    if ([deviceString isEqualToString:@"iPhone5,2"])    return 52; // @"iPhone 5";
    if ([deviceString isEqualToString:@"iPhone5,3"])    return 53; // @"iPhone 5C";
    if ([deviceString isEqualToString:@"iPhone5,4"])    return 54; // @"iPhone 5C";
    if ([deviceString isEqualToString:@"iPhone6,1"])    return 61; // @"iPhone 5S";
    if ([deviceString isEqualToString:@"iPhone6,2"])    return 62; // @"iPhone 5S";
    if ([deviceString isEqualToString:@"iPhone7,1"])    return 71; // @"iPhone 6 Plus";
    if ([deviceString isEqualToString:@"iPhone7,2"])    return 72; // @"iPhone 6";
    if ([deviceString isEqualToString:@"iPhone8,1"])    return 81; // @"iPhone 6s";
    if ([deviceString isEqualToString:@"iPhone8,2"])    return 82; // @"iPhone 6s Plus";
    if ([deviceString isEqualToString:@"iPhone9,1"])    return 91; // @"iPhone 6s";
    if ([deviceString isEqualToString:@"iPhone9,2"])    return 92; // @"iPhone 6s Plus";
    
    //iPod
    if ([deviceString isEqualToString:@"iPod1,1"]) return 11; // @"iPod Touch 1G";
    if ([deviceString isEqualToString:@"iPod2,1"]) return 21; // @"iPod Touch 2G";
    if ([deviceString isEqualToString:@"iPod3,1"]) return 31; // @"iPod Touch 3G";
    if ([deviceString isEqualToString:@"iPod4,1"]) return 41; // @"iPod Touch 4G";
    if ([deviceString isEqualToString:@"iPod5,1"]) return 51; // @"iPod Touch 5G";
    
    //iPad
    if ([deviceString isEqualToString:@"iPad1,1"])      return 11; // @"iPad";
    if ([deviceString isEqualToString:@"iPad2,1"])      return 21; // @"iPad 2 (WiFi)";
    if ([deviceString isEqualToString:@"iPad2,2"])      return 22; // @"iPad 2 (GSM)";
    if ([deviceString isEqualToString:@"iPad2,3"])      return 23; // @"iPad 2 (CDMA)";
    if ([deviceString isEqualToString:@"iPad2,4"])      return 24; // @"iPad 2 (32nm)";
    if ([deviceString isEqualToString:@"iPad2,5"])      return 25; // @"iPad mini (WiFi)";
    if ([deviceString isEqualToString:@"iPad2,6"])      return 26; // @"iPad mini (GSM)";
    if ([deviceString isEqualToString:@"iPad2,7"])      return 27; // @"iPad mini (CDMA)";
    
    if ([deviceString isEqualToString:@"iPad3,1"])      return 31; // @"iPad 3(WiFi)";
    if ([deviceString isEqualToString:@"iPad3,2"])      return 32; // @"iPad 3(CDMA)";
    if ([deviceString isEqualToString:@"iPad3,3"])      return 33; // @"iPad 3(4G)";
    if ([deviceString isEqualToString:@"iPad3,4"])      return 34; // @"iPad 4 (WiFi)";
    if ([deviceString isEqualToString:@"iPad3,5"])      return 35; // @"iPad 4 (4G)";
    if ([deviceString isEqualToString:@"iPad3,6"])      return 36; // @"iPad 4 (CDMA)";
    
    if ([deviceString isEqualToString:@"iPad4,1"])      return 41; // @"iPad Air";
    if ([deviceString isEqualToString:@"iPad4,2"])      return 42; // @"iPad Air";
    if ([deviceString isEqualToString:@"iPad4,3"])      return 43; // @"iPad Air";
    if ([deviceString isEqualToString:@"iPad5,3"])      return 53; // @"iPad Air 2";
    if ([deviceString isEqualToString:@"iPad5,4"])      return 54; // @"iPad Air 2";
    if ([deviceString isEqualToString:@"i386"])         return -1; // @"Simulator";
    if ([deviceString isEqualToString:@"x86_64"])       return -1; // @"Simulator";
    
    if ([deviceString isEqualToString:@"iPad4,4"]||[deviceString isEqualToString:@"iPad4,5"]||[deviceString isEqualToString:@"iPad4,6"])  return 46; // @"iPad mini 2";
    if ([deviceString isEqualToString:@"iPad4,7"]||[deviceString isEqualToString:@"iPad4,8"]||[deviceString isEqualToString:@"iPad4,9"])  return 49;  // @"iPad mini 3";
    // deviceString;
    
    return -1;
}

const char* mc::bundle_id(){
    return [[[NSBundle mainBundle] bundleIdentifier] UTF8String];
}

const char* mc::package_name(){
    return NULL;
}

int uidBaseIncrease = 0;
void mc::guid(char objectid[BS_UNIQUE_ID_LENGTH]) {
    unsigned char uid[12];
    timeval start;
    gettimeofday(&start, NULL);
    if (uidBaseIncrease <= 0) {
        srand((unsigned)time(0));
        uidBaseIncrease = rand();
    } else {
        uidBaseIncrease += 1;
    }
    short a = 1;
    if (((char*)&a)[0] == 1) {
        uid[0] = ((char*)&start.tv_sec)[3];
        uid[1] = ((char*)&start.tv_sec)[2];
        uid[2] = ((char*)&start.tv_sec)[1];
        uid[3] = ((char*)&start.tv_sec)[0];
        uid[7] = ((char*)&start.tv_sec)[1];
        uid[8] = ((char*)&start.tv_sec)[0];
        uid[9] = ((char*)&uidBaseIncrease)[2];
        uid[10] = ((char*)&uidBaseIncrease)[1];
        uid[11] = ((char*)&uidBaseIncrease)[0];
    } else {
        *(uint32_t*)uid = start.tv_sec;
        memcpy(uid+7, &start.tv_usec, 2);
        memcpy(uid+9, &uidBaseIncrease, 3);
    }
    //memcpy(uid, &start.tv_sec, 4);
    
    const char* device = mc::device_id();
    uint32_t crc = bs_crc32(0, (void*)device, (uint32_t)strlen(device));
    memcpy(uid+4, &crc, 3);
    *(uint16_t*)(uid+7) = start.tv_usec;
    
    char id[BS_UNIQUE_ID_LENGTH];
    for (int i=0; i<12; i++) {
        sprintf(id+i*2, "%02x", uid[i]);
    }
    
    
    id[BS_UNIQUE_ID_EFFECT_LENGTH] = '\0';
    snprintf(objectid, BS_UNIQUE_ID_LENGTH, "%s", id);
}
