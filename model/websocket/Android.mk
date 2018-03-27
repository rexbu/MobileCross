LOCAL_PATH := $(call my-dir)

# TARGET_ARCH_ABI == armeabi-v7a || x86 只编译32位版本
ifeq ($(TARGET_ARCH_ABI),$(filter $(TARGET_ARCH_ABI), armeabi-v7a x86))

include $(CLEAR_VARS)

LOCAL_CFLAGS += -D__ANDROID__ -D__DEBUG__ -g

LOCAL_C_INCLUDES	:=	\
    $(LOCAL_PATH)/../../bs  \
    $(LOCAL_PATH)/../../basic  \
    $(LOCAL_PATH)/../../3rdparty/openssl   \
    $(LOCAL_PATH)/../../3rdparty/websocket/include

SRCFILES = $(wildcard $(LOCAL_PATH)/*.cpp $(LOCAL_PATH)/*/*.cpp)
SRCS = $(patsubst $(LOCAL_PATH)/%, ./%,$(SRCFILES)) 

LOCAL_SRC_FILES += $(SRCS)

LOCAL_MODULE := ws

include $(BUILD_STATIC_LIBRARY)

endif
