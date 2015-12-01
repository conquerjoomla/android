###################
# project64-core
###################
include $(CLEAR_VARS)
LOCAL_PATH := $(JNI_LOCAL_PATH)
CORE_SRC := $(PJ64_SRC)/Project64-core

LOCAL_MODULE := project64-core
LOCAL_STATIC_LIBRARIES := common
LOCAL_ARM_MODE := arm

LOCAL_C_INCLUDES :=                            \
    $(SDL_INCLUDES)                            \
    E:\Android\project64-console\Source        \

LOCAL_SRC_FILES :=                             \
    $(CORE_SRC)/AppInit.cpp                    \

LOCAL_CFLAGS :=                                \
    $(COMMON_CFLAGS)                           \
    -DANDROID                                  \
    -DNO_ASM                                   \

LOCAL_LDFLAGS :=                               \
    -Wl,-Bsymbolic                             \
    -Wl,-export-dynamic                        \

LOCAL_CPPFLAGS := $(COMMON_CPPFLAGS)           \

include $(BUILD_STATIC_LIBRARY)
