#########################
# project64-core
#########################
include $(CLEAR_VARS)
LOCAL_PATH := $(JNI_LOCAL_PATH)
SRCDIR := ./Project64-core

LOCAL_MODULE := project64-core
LOCAL_STATIC_LIBRARIES := common

LOCAL_C_INCLUDES := ../     \
    ../3rdParty/            \
    $(SDL_INCLUDES)         \
               
LOCAL_SRC_FILES :=                                                     \
    $(SRCDIR)/AppInit.cpp                                              \
    $(SRCDIR)/Multilanguage/LanguageClass.cpp                          \
    $(SRCDIR)/Settings/LoggingSettings.cpp                             \
    $(SRCDIR)/Settings/SettingsClass.cpp                               \
    $(SRCDIR)/N64System/Interpreter/InterpreterCPU.cpp                 \
    $(SRCDIR)/N64System/Interpreter/InterpreterOps.cpp                 \
    $(SRCDIR)/N64System/Mips/Audio.cpp                                 \
    $(SRCDIR)/N64System/Mips/Dma.cpp                                   \
    $(SRCDIR)/N64System/Mips/MemoryVirtualMem.cpp                      \
    $(SRCDIR)/N64System/Mips/RegisterClass.cpp                         \
    $(SRCDIR)/N64System/Mips/SystemEvents.cpp                          \
    $(SRCDIR)/N64System/Mips/SystemTiming.cpp                          \
    $(SRCDIR)/N64System/Mips/TLBclass.cpp                              \
    $(SRCDIR)/N64System/Recompiler/RecompilerClass.cpp                 \
    $(SRCDIR)/N64System/Recompiler/RecompilerMemory.cpp                \
    $(SRCDIR)/N64System/Recompiler/RegInfo.cpp                         \
    $(SRCDIR)/N64System/Recompiler/X86ops.cpp                          \
    $(SRCDIR)/N64System/CheatClass.cpp                                 \
    $(SRCDIR)/N64System/FramePerSecondClass.cpp                        \
    $(SRCDIR)/N64System/N64Class.cpp                                   \
    $(SRCDIR)/N64System/N64RomClass.cpp                                \
    $(SRCDIR)/N64System/ProfilingClass.cpp                             \
    $(SRCDIR)/N64System/SpeedLimiterClass.cpp                          \
    $(SRCDIR)/N64System/SystemGlobals.cpp                              \
    $(SRCDIR)/Settings/SettingType/SettingsType-Application.cpp        \
    $(SRCDIR)/Settings/SettingType/SettingsType-ApplicationPath.cpp    \
    $(SRCDIR)/Settings/SettingType/SettingsType-Cheats.cpp             \
    $(SRCDIR)/Settings/SettingType/SettingsType-GameSetting.cpp        \
    $(SRCDIR)/Settings/SettingType/SettingsType-RelativePath.cpp       \
    $(SRCDIR)/Settings/SettingType/SettingsType-RDBCpuType.cpp         \
    $(SRCDIR)/Settings/SettingType/SettingsType-RDBOnOff.cpp           \
    $(SRCDIR)/Settings/SettingType/SettingsType-RDBRamSize.cpp         \
    $(SRCDIR)/Settings/SettingType/SettingsType-RDBSaveChip.cpp        \
    $(SRCDIR)/Settings/SettingType/SettingsType-RDBYesNo.cpp           \
    $(SRCDIR)/Settings/SettingType/SettingsType-RomDatabase.cpp        \
    $(SRCDIR)/Settings/SettingType/SettingsType-RomDatabaseIndex.cpp   \
    $(SRCDIR)/Settings/SettingType/SettingsType-TempBool.cpp           \
    $(SRCDIR)/Settings/SettingType/SettingsType-TempNumber.cpp         \
    $(SRCDIR)/Settings/SettingType/SettingsType-TempString.cpp         \
    $(SRCDIR)/Settings/DebugSettings.cpp                               \
    $(SRCDIR)/Settings/GameSettings.cpp                                \
    $(SRCDIR)/Settings/N64SystemSettings.cpp                           \

LOCAL_CFLAGS := $(COMMON_CFLAGS)
LOCAL_CPPFLAGS := $(COMMON_CPPFLAGS)

include $(BUILD_STATIC_LIBRARY)
