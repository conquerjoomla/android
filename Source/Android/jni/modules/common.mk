#########################
# common
#########################
include $(CLEAR_VARS)
LOCAL_PATH := $(JNI_LOCAL_PATH)
SRCDIR := ../..
PRJDIR := $(SRCDIR)/Common

LOCAL_MODULE := common
LOCAL_ARM_MODE := arm

LOCAL_C_INCLUDES :=                            \
    E:/Android/project64-console/Source        \
    $(SDL_INCLUDES)                            \

LOCAL_SRC_FILES :=                             \
    $(PRJDIR)/StdString.cpp                    \

LOCAL_CFLAGS :=                                \
    $(COMMON_CFLAGS)                           \
    -DANDROID                                  \
    -DNO_ASM                                   \

LOCAL_LDFLAGS :=                               \
    -Wl,-Bsymbolic                             \
    -Wl,-export-dynamic                        \

LOCAL_CPPFLAGS := $(COMMON_CPPFLAGS)           \

include $(BUILD_STATIC_LIBRARY)