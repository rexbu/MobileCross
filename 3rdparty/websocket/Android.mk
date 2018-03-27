# get current directory
LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_ARCH_ABI),$(filter $(TARGET_ARCH_ABI), armeabi-v7a x86))

include $(CLEAR_VARS)
LOCAL_MODULE := websockets
LOCAL_SRC_FILES := $(LOCAL_PATH)/$(TARGET_ARCH_ABI)/libwebsockets.a
include $(PREBUILT_STATIC_LIBRARY)

endif