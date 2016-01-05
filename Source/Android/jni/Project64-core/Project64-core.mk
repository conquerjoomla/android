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
    $(SRCDIR)/Settings/SettingsClass.cpp                               \
    $(SRCDIR)/N64System/N64Class.cpp                                   \
    $(SRCDIR)/N64System/N64RomClass.cpp                                \
    $(SRCDIR)/N64System/SystemGlobals.cpp                              \
    $(SRCDIR)/Settings/DebugSettings.cpp                               \
    $(SRCDIR)/Settings/GameSettings.cpp                                \
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
    $(SRCDIR)/Settings/SettingType/SettingsType-TempString.cpp

LOCAL_CFLAGS := $(COMMON_CFLAGS)
LOCAL_CPPFLAGS := $(COMMON_CPPFLAGS)

include $(BUILD_STATIC_LIBRARY)
