#########################
# project64-console
#########################
include $(CLEAR_VARS)
LOCAL_PATH := $(JNI_LOCAL_PATH)
SRCDIR := ../..
PRJDIR := $(SRCDIR)/Project64-console

LOCAL_MODULE := project64-console
LOCAL_STATIC_LIBRARIES := project64-core
#LOCAL_ARM_MODE := arm

LOCAL_C_INCLUDES :=         \
    E:/Android/project64-console/Source               \
    $(SDL_INCLUDES)         \

LOCAL_SRC_FILES :=                      \
    $(PRJDIR)/main.cpp                  \

LOCAL_CFLAGS :=                                 \
    $(COMMON_CFLAGS)                            \
    -DANDROID                                   \
    -DNO_ASM                                    \

LOCAL_CPPFLAGS := $(COMMON_CPPFLAGS)

LOCAL_LDLIBS := -llog

include $(BUILD_STATIC_LIBRARY)
