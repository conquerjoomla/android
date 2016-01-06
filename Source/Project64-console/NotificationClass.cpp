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
#include <common/StdString.h>
#include <Common/Trace.h>
#ifdef _WIN32
#include <Windows.h>
#endif

#if defined(ANDROID)
#include <android/log.h>

#define printf(...) __android_log_print(ANDROID_LOG_VERBOSE, "UI-Console", __VA_ARGS__)
#endif

CNotificationImp & Notify(void)
{
    static CNotificationImp g_Notify;
    return g_Notify;
}

CNotificationImp::CNotificationImp()
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

#ifdef _WIN32
void CNotificationImp::DisplayError(const char * Message) const
{
    if (this == NULL) { return; }

    WriteTrace(TraceUserInterface, TraceError, Message);
#ifdef tofix
    WindowMode();
#endif

    HWND Parent = NULL;
#ifdef tofix
    if (m_hWnd)
    {
        Parent = reinterpret_cast<HWND>(m_hWnd->GetWindowHandle());
    }
#endif
    MessageBoxW(Parent, stdstr(Message).ToUTF16().c_str(), wGS(MSG_MSGBOX_TITLE).c_str(), MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
}

void CNotificationImp::FatalError(const char * Message) const
{
    DisplayMessage(0,Message);
}

//User Feedback
void CNotificationImp::DisplayMessage(int /*DisplayTime*/, const char * Message) const
{
    printf("%ws\n",Message);
}

void CNotificationImp::DisplayMessage2(const char * /*Message*/) const
{
}

// Ask a Yes/No Question to the user, yes = true, no = false
bool CNotificationImp::AskYesNoQuestion(const char * /*Question*/) const
{
    return false;
}
#else
void CNotificationImp::DisplayError(const char * Message) const
{
    DisplayMessage(10,Message);
}

void CNotificationImp::FatalError(const char * Message) const
{
    DisplayMessage(10,Message);
}

void CNotificationImp::DisplayMessage(int DisplayTime, const char * Message) const
{
    printf("%s\n",Message);
}
#endif

void CNotificationImp::BreakPoint(const char * FileName, int32_t LineNumber)
{
    DisplayError(stdstr_f("Break point found at\n%s\n%d", FileName, LineNumber).c_str());
#ifdef _WIN32
    if (IsDebuggerPresent() != 0)
    {
        DebugBreak();
    }
    else
    {
#ifdef tofix
        if (g_BaseSystem)
        {
            g_BaseSystem->CloseCpu();
        }
#endif
    }
#else
    DisplayError(notice.c_str());
    __builtin_trap();
#endif
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