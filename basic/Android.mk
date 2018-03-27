LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -D__ANDROID__ -g -DDEBUG

LOCAL_C_INCLUDES =	\
	$(LOCAL_PATH)/../3rdparty/libzip  \
	$(LOCAL_PATH)/../bs \
	$(LOCAL_PATH)/

SRCFILES = $(wildcard $(LOCAL_PATH)/*.cpp $(LOCAL_PATH)/*/*.cpp)
SRCS = $(patsubst $(LOCAL_PATH)/%, ./%,$(SRCFILES))

LOCAL_SRC_FILES += \
	$(SRCS)

#LOCAL_SHARED_LIBRARIES := libnative
#LOCAL_LDLIBS    := -llog -landroid -lz -ldl

LOCAL_MODULE := basic_d

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -D__ANDROID__ -g
#APP_PLATFORM := android-19

LOCAL_C_INCLUDES =	\
	$(LOCAL_PATH)/../3rdparty/libzip  \
	$(LOCAL_PATH)/../bs \
	$(LOCAL_PATH)/

SRCFILES = $(wildcard $(LOCAL_PATH)/*.cpp $(LOCAL_PATH)/*/*.cpp)
SRCS = $(patsubst $(LOCAL_PATH)/%, ./%,$(SRCFILES))

LOCAL_SRC_FILES += \
	$(SRCS)

LOCAL_MODULE := basic

include $(BUILD_STATIC_LIBRARY)