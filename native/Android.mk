LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE_FILENAME = libnative
LOCAL_CFLAGS	:= -D__ANDROID__ -D__DEBUG__ -g

# 源文件
SRCFILES = $(wildcard $(LOCAL_PATH)/*.cpp $(LOCAL_PATH)/*/*.cpp)
SRCS = $(patsubst $(LOCAL_PATH)/%, ./%,$(SRCFILES))

LOCAL_SRC_FILES += $(SRCS)

LOCAL_LDLIBS    := -llog -landroid -ldl
LOCAL_MODULE    := native

include $(BUILD_SHARED_LIBRARY)

