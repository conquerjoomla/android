######################
# Project64-bridge
######################
include $(CLEAR_VARS)
LOCAL_PATH := $(JNI_LOCAL_PATH)
SRCDIR := ./Project64-bridge

LOCAL_MODULE := Project64-bridge
LOCAL_STATIC_LIBRARIES := common  \
      Project64-core              \

LOCAL_C_INCLUDES := $(SDL_INCLUDES)

LOCAL_SRC_FILES :=                   \
    $(SRCDIR)/jniBridge.cpp          \
    $(SRCDIR)/jniBridgeSettings.cpp  \
    $(SRCDIR)/NotificationClass.cpp  \

LOCAL_CFLAGS :=         \
    $(COMMON_CFLAGS)    \
    -DUSE_GLES          \

LOCAL_CPPFLAGS := $(COMMON_CPPFLAGS)

LOCAL_LDLIBS :=         \
    -lGLESv2            \
    -llog               \

include $(BUILD_SHARED_LIBRARY)
