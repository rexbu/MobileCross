# version libiconv-1.15

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := iconv

LOCAL_CFLAGS    := \
	-Wno-multichar \
	-D_ANDROID \
	-DBUILDING_LIBICONV \
	-DIN_LIBRARY \
	-DLIBDIR="\"c\"" \
    -I$(LOCAL_PATH)/ \
    -I$(LOCAL_PATH)/include/ \
    -I$(LOCAL_PATH)/lib/ \
	-I$(LOCAL_PATH)/libcharset/include \

LOCAL_SRC_FILES := \
     $(LOCAL_PATH)/lib/iconv.c \
     $(LOCAL_PATH)/lib/relocatable.c \
     $(LOCAL_PATH)/libcharset/lib/localcharset.c

include $(BUILD_STATIC_LIBRARY)