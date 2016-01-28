LOCAL_PATH := $(call my-dir)

########## AE-EXPORTS ##########

include $(CLEAR_VARS)

LOCAL_MODULE := ae-exports

LOCAL_C_INCLUDES := $(SDL_INCLUDES)

LOCAL_SRC_FILES := ae_exports.cpp

LOCAL_CFLAGS := $(COMMON_CFLAGS)

LOCAL_CPPFLAGS := $(COMMON_CPPFLAGS)

LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)
