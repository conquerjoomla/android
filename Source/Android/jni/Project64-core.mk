###################
# project64-core
###################
include $(CLEAR_VARS)
LOCAL_PATH := $(JNI_LOCAL_PATH)
SRCDIR := ../..
PRJDIR := $(SRCDIR)/Project64-core

LOCAL_MODULE := project64-core
LOCAL_STATIC_LIBRARIES := common
LOCAL_ARM_MODE := arm

LOCAL_C_INCLUDES :=                            \
    E:/Android/project64-console/Source        \
    $(SDL_INCLUDES)                            \

LOCAL_SRC_FILES :=                             \
    $(PRJDIR)/AppInit.cpp                      \

LOCAL_CFLAGS :=                                \
    $(COMMON_CFLAGS)                           \
    -DANDROID                                  \
    -DNO_ASM                                   \

LOCAL_LDFLAGS :=                               \
    -Wl,-Bsymbolic                             \
    -Wl,-export-dynamic                        \

LOCAL_CPPFLAGS := $(COMMON_CPPFLAGS)           \

LOCAL_LDLIBS := -llog

include $(BUILD_STATIC_LIBRARY)
