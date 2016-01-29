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
#include <common/StdString.h>
#include <Common/Trace.h>
#include <Project64-core/N64System/SystemGlobals.h>
#include <Project64-core/Settings/SettingsClass.h>
#include "NotificationClass.h"
#if defined(ANDROID)
#include <android/log.h>

#define printf(...) __android_log_print(ANDROID_LOG_VERBOSE, "PJ64-Bridge", __VA_ARGS__)
#endif

CNotificationImp & Notify(void)
{
    static CNotificationImp g_Notify;
    return g_Notify;
}

CNotificationImp::CNotificationImp()
{
}

CNotificationImp::~CNotificationImp()
{
}

void CNotificationImp::DisplayError(const char * /*Message*/) const
{
}

void CNotificationImp::DisplayError(LanguageStringID /*StringID*/) const
{
}

void CNotificationImp::FatalError(LanguageStringID /*StringID*/) const
{
}

void CNotificationImp::DisplayMessage(int /*DisplayTime*/, LanguageStringID /*StringID*/) const
{
}

void CNotificationImp::FatalError(const char * Message) const
{
    DisplayMessage(0,Message);
}

//User Feedback
void CNotificationImp::DisplayMessage(int /*DisplayTime*/, const char * Message) const
{
    printf("%s\n",Message);
}

#ifdef _WIN32
void CNotificationImp::DisplayMessage2(const char * /*Message*/) const
{
}

// Ask a Yes/No Question to the user, yes = true, no = false
bool CNotificationImp::AskYesNoQuestion(const char * /*Question*/) const
{
    return false;
}
#endif

void CNotificationImp::BreakPoint(const char * FileName, int32_t LineNumber)
{
    if (g_Settings->LoadBool(Debugger_Enabled))
    {
        DisplayError(stdstr_f("Break point found at\n%s\n%d", FileName, LineNumber).c_str());
#ifndef _WIN32
		__builtin_trap();
#endif
#ifdef tofix
		if (g_BaseSystem) 
		{
			g_BaseSystem->CloseCpu();
		}
#endif
    }
    else
    {
        DisplayError("Fatal Error: Stopping emulation");
#ifdef tofix
		if (g_BaseSystem) 
		{
			g_BaseSystem->CloseCpu();
		}
#endif
	}
}

void CNotificationImp::AppInitDone(void)
{
}

bool CNotificationImp::ProcessGuiMessages(void) const
{
    return true;
}

void CNotificationImp::ChangeFullScreen(void) const
{
}