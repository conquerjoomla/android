JNI_LOCAL_PATH := $(call my-dir)

AE_BRIDGE_INCLUDES := $(JNI_LOCAL_PATH)/ae-bridge/
SDL_INCLUDES := $(JNI_LOCAL_PATH)/SDL2/include/
PJ64_SRC := $(JNI_LOCAL_PATH)/

COMMON_CFLAGS :=                    \
    -O3                             \
    -ffast-math                     \
    -fno-strict-aliasing            \
    -fomit-frame-pointer            \
    -frename-registers              \
    -fsingle-precision-constant     \
    -fvisibility=hidden             \
    -DANDROID                       \
    -DNO_ASM                        \

COMMON_CPPFLAGS :=                  \
    -fvisibility-inlines-hidden     \
    -fexceptions                    \

include $(JNI_LOCAL_PATH)/Common/common.mk
include $(JNI_LOCAL_PATH)/RSP-hle/rsp-hle.mk
include $(JNI_LOCAL_PATH)/Project64-core/Project64-core.mk
include $(JNI_LOCAL_PATH)/Project64-console/Project64-console.mk
include $(JNI_LOCAL_PATH)/ae-bridge/Android.mk
