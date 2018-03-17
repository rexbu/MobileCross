# get current directory
LOCAL_PATH := $(call my-dir)

# libssl.a
#
include $(CLEAR_VARS)
LOCAL_MODULE := libssl
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libssl.a
include $(PREBUILT_STATIC_LIBRARY)

# libwebsockets.a
#
include $(CLEAR_VARS)
LOCAL_MODULE := libwebsockets
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libwebsockets.a
include $(PREBUILT_STATIC_LIBRARY)

