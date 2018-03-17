LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_CFLAGS += -D__ANDROID__ -g -DDEBUG
#APP_PLATFORM := android-19

SRCFILES = $(wildcard $(LOCAL_PATH)/*.cpp $(LOCAL_PATH)/*/*.cpp)
SRCS = $(patsubst $(LOCAL_PATH)/%, ./%,$(SRCFILES)) 

LOCAL_C_INCLUDES =	\
	$(LOCAL_PATH)/../3rdparty/libzip		\
	$(LOCAL_PATH)/../bs 			\
	$(LOCAL_PATH)/../basic
	
LOCAL_SRC_FILES += \
	$(SRCS)

LOCAL_SHARED_LIBRARIES := libnative
LOCAL_STATIC_LIBRARIES := \
    libasync    \
    libbasic_d    \
	libbs		\
	libzip

LOCAL_LDLIBS    := -ldl -llog -lz
LOCAL_MODULE    := MeCloud_d

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -D__ANDROID__ -g
#APP_PLATFORM := android-19

SRCFILES = $(wildcard $(LOCAL_PATH)/*.cpp $(LOCAL_PATH)/*/*.cpp)
SRCS = $(patsubst $(LOCAL_PATH)/%, ./%,$(SRCFILES))

LOCAL_C_INCLUDES =	\
	$(LOCAL_PATH)/../3rdparty/libzip		\
	$(LOCAL_PATH)/../bs 			\
	$(LOCAL_PATH)/../basic

LOCAL_SRC_FILES += \
	$(SRCS)

LOCAL_SHARED_LIBRARIES := libnative
LOCAL_STATIC_LIBRARIES := \
    libasync    \
    libbasic_r    \
	libbs		\
	libzip

LOCAL_LDLIBS    := -ldl -llog -lz
LOCAL_MODULE    := MeCloud_r

include $(BUILD_SHARED_LIBRARY)