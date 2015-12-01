#########################
# project64-console
#########################
include $(CLEAR_VARS)
LOCAL_PATH := $(JNI_LOCAL_PATH)
CONSOLE_SRC := $(PJ64_SRC)/Project64-console

LOCAL_MODULE := project64-console
LOCAL_STATIC_LIBRARIES := project64-core common
#LOCAL_ARM_MODE := arm

LOCAL_C_INCLUDES :=         \
    $(SDL_INCLUDES)         \
    $(PJ64_SRC)             \

LOCAL_SRC_FILES :=                      \
    $(CONSOLE_SRC)/main.cpp                  \

LOCAL_CFLAGS :=                                 \
    $(COMMON_CFLAGS)                            \
    -DANDROID                                   \
    -DNO_ASM                                    \

LOCAL_CPPFLAGS := $(COMMON_CPPFLAGS)

LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)
