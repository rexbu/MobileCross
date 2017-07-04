LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -D__ANDROID__ -g
#APP_PLATFORM := android-19

SRCFILES = $(wildcard $(LOCAL_PATH)/*.cpp $(LOCAL_PATH)/*/*.cpp)
SRCS = $(patsubst $(LOCAL_PATH)/%, ./%,$(SRCFILES)) 

LOCAL_C_INCLUDES =	\
	$(LOCAL_PATH)/../../third/		\
	$(LOCAL_PATH)/../../third/cjson	\
	$(LOCAL_PATH)/../../third/curl	\
	$(LOCAL_PATH)/../../third/curl/include	\
	$(LOCAL_PATH)/../../third/curl/include/curl	\
	$(LOCAL_PATH)/../../third/openssl	\
	$(LOCAL_PATH)/../../third/openssl/include	\
	$(LOCAL_PATH)/../../third/openssl/include/openssl	\
	$(LOCAL_PATH)/../../third/sszip	\
	$(LOCAL_PATH)/../../third/sszip/aes	\
	$(LOCAL_PATH)/../../third/sszip/minizip	\
	$(LOCAL_PATH)/../../third/libzip	\
	$(LOCAL_PATH)/../bs
	
LOCAL_SRC_FILES += \
	$(SRCS)

LOCAL_MODULE := basic

include $(BUILD_STATIC_LIBRARY)
