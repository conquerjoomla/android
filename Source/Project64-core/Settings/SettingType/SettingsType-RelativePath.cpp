/****************************************************************************
*                                                                           *
* Project64 - A Nintendo 64 emulator.                                      *
* http://www.pj64-emu.com/                                                  *
* Copyright (C) 2012 Project64. All rights reserved.                        *
*                                                                           *
* License:                                                                  *
* GNU/GPLv2 http://www.gnu.org/licenses/gpl-2.0.html                        *
*                                                                           *
****************************************************************************/
#include "stdafx.h"
#include "SettingsType-RelativePath.h"

#if defined(ANDROID)
#include <android/log.h>

#define printf(...) __android_log_print(ANDROID_LOG_VERBOSE, "UI-Console", __VA_ARGS__)
#endif

CSettingTypeRelativePath::CSettingTypeRelativePath(const char * Directory, const char * FileName) :
m_Directory(Directory),
m_FileName(FileName)
{
    BuildPath();
    g_Settings->RegisterChangeCB(Cmd_BaseDirectory, this, RefreshSettings);
}

CSettingTypeRelativePath::~CSettingTypeRelativePath(void)
{
    g_Settings->RegisterChangeCB(Cmd_BaseDirectory, this, RefreshSettings);
}

bool CSettingTypeRelativePath::Load(int /*Index*/, stdstr & value) const
{
    value = m_FullPath;
    return true;
}

//return the default values
void CSettingTypeRelativePath::LoadDefault(int /*Index*/, bool & /*Value*/) const
{
    g_Notify->BreakPoint(__FILE__, __LINE__);
}

void CSettingTypeRelativePath::LoadDefault(int /*Index*/, uint32_t & /*Value*/) const
{
    g_Notify->BreakPoint(__FILE__, __LINE__);
}

void CSettingTypeRelativePath::LoadDefault(int /*Index*/, stdstr & /*Value*/) const
{
    g_Notify->BreakPoint(__FILE__, __LINE__);
}

void CSettingTypeRelativePath::Save(int /*Index*/, bool /*Value*/)
{
    g_Notify->BreakPoint(__FILE__, __LINE__);
}

void CSettingTypeRelativePath::Save(int /*Index*/, uint32_t /*Value*/)
{
    g_Notify->BreakPoint(__FILE__, __LINE__);
}

void CSettingTypeRelativePath::Save(int /*Index*/, const stdstr & Value)
{
    m_Directory = "";
    m_FileName = Value;
    BuildPath();
}

void CSettingTypeRelativePath::Save(int /*Index*/, const char * Value)
{
    m_Directory = "";
    m_FileName = Value;
    BuildPath();
}

void CSettingTypeRelativePath::Delete(int /*Index*/)
{
    g_Notify->BreakPoint(__FILE__, __LINE__);
}

void CSettingTypeRelativePath::BuildPath(void)
{
    CPath FullPath;

#ifdef _WIN32
    FullPath.SetDriveDirectory(g_Settings->LoadStringVal(Cmd_BaseDirectory).c_str());
#else
    FullPath.SetDirectory(g_Settings->LoadStringVal(Cmd_BaseDirectory).c_str());
#endif
    FullPath.AppendDirectory(m_Directory.c_str());
    FullPath.SetNameExtension(m_FileName.c_str());
    m_FullPath = (const char *)FullPath;
    printf("g_Settings->LoadStringVal(Cmd_BaseDirectory).c_str() = %s\n", g_Settings->LoadStringVal(Cmd_BaseDirectory).c_str());
    printf("m_FullPath = %s\n", (const char *)FullPath);
}

void CSettingTypeRelativePath::RefreshSettings(void * _this)
{
    ((CSettingTypeRelativePath *)_this)->BuildPath();
}