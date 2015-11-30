JNI_LOCAL_PATH := $(call my-dir)

AE_BRIDGE_INCLUDES := $(JNI_LOCAL_PATH)/ae-bridge/
SDL_INCLUDES := $(JNI_LOCAL_PATH)/SDL2/include/

COMMON_CFLAGS :=                    \
    -O3                             \
    -ffast-math                     \
    -fno-strict-aliasing            \
    -fomit-frame-pointer            \
    -frename-registers              \
    -fsingle-precision-constant     \
    -fvisibility=hidden             \

COMMON_CPPFLAGS :=                  \
    -fvisibility-inlines-hidden     \
    -fexceptions                    \

include $(JNI_LOCAL_PATH)/SDL2/Android.mk
include $(JNI_LOCAL_PATH)/ae-bridge/Android.mk
include $(JNI_LOCAL_PATH)/common.mk
include $(JNI_LOCAL_PATH)/Project64-core.mk
include $(JNI_LOCAL_PATH)/Project64-console.mk