#########################
# common
#########################
include $(CLEAR_VARS)
LOCAL_PATH := $(JNI_LOCAL_PATH)
COMMON_SRC := $(PJ64_SRC)/Common

LOCAL_MODULE := common
LOCAL_ARM_MODE := arm

LOCAL_C_INCLUDES :=                            \
    $(SDL_INCLUDES)                            \
    $(PJ64_SRC)                                \

LOCAL_SRC_FILES :=                             \
    $(COMMON_SRC)/StdString.cpp                \

LOCAL_CFLAGS :=                                \
    $(COMMON_CFLAGS)                           \
    -DANDROID                                  \
    -DNO_ASM                                   \

LOCAL_CFLAGS := -IE:/Android/project64-console/Source \

LOCAL_LDFLAGS :=                               \
    -Wl,-Bsymbolic                             \
    -Wl,-export-dynamic                        \

include $(BUILD_STATIC_LIBRARY)