#########################
# mupen64plus-ui-console
#########################
include $(CLEAR_VARS)
LOCAL_PATH := $(JNI_LOCAL_PATH)
SRCDIR := ../../Project64-console

LOCAL_MODULE := project64-console
LOCAL_SHARED_LIBRARIES := ae-imports
#LOCAL_ARM_MODE := arm

LOCAL_C_INCLUDES := $(SDL_INCLUDES)

LOCAL_SRC_FILES :=                      \
    $(SRCDIR)/main.cpp                  \

LOCAL_CFLAGS :=                                 \
    $(COMMON_CFLAGS)                            \
    -DANDROID                                   \
    -DNO_ASM                                    \

LOCAL_CPPFLAGS := $(COMMON_CPPFLAGS)

LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)
