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

include $(JNI_LOCAL_PATH)/3rdParty/png/png.mk
include $(JNI_LOCAL_PATH)/3rdParty/zlib/zlib.mk
include $(JNI_LOCAL_PATH)/Common/common.mk
include $(JNI_LOCAL_PATH)/Glide64/Glide64.mk
include $(JNI_LOCAL_PATH)/PluginAudio/PluginAudio.mk
include $(JNI_LOCAL_PATH)/PluginInput/PluginInput.mk
include $(JNI_LOCAL_PATH)/PluginRSP/PluginRSP.mk
include $(JNI_LOCAL_PATH)/Project64-bridge/Project64-bridge.mk
include $(JNI_LOCAL_PATH)/Project64-core/Project64-core.mk
include $(JNI_LOCAL_PATH)/SDL2/Android.mk
include $(JNI_LOCAL_PATH)/Settings/Settings.mk