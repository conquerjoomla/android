#########################
# project64-console
#########################
include $(CLEAR_VARS)
LOCAL_PATH := $(JNI_LOCAL_PATH)
SRCDIR := ./Project64-console

LOCAL_MODULE := project64-console
LOCAL_STATIC_LIBRARIES := project64-core common

LOCAL_C_INCLUDES := ../     \
    $(SDL_INCLUDES)         \
               
LOCAL_SRC_FILES :=                      \
    $(SRCDIR)/main.cpp                  \
    $(SRCDIR)/NotificationClass.cpp     \

LOCAL_CFLAGS := $(COMMON_CFLAGS)
LOCAL_CPPFLAGS := $(COMMON_CPPFLAGS)

LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)
