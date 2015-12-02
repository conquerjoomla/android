#########################
# project64-core
#########################
include $(CLEAR_VARS)
LOCAL_PATH := $(JNI_LOCAL_PATH)
SRCDIR := ./Project64-core

LOCAL_MODULE := project64-core
LOCAL_STATIC_LIBRARIES := common

LOCAL_C_INCLUDES := ../     \
    $(SDL_INCLUDES)         \
               
LOCAL_SRC_FILES :=                          \
    $(SRCDIR)/AppInit.cpp                   \
    $(SRCDIR)/Settings/SettingsClass.cpp    \

LOCAL_CFLAGS := $(COMMON_CFLAGS)
LOCAL_CPPFLAGS := $(COMMON_CPPFLAGS)

include $(BUILD_STATIC_LIBRARY)
