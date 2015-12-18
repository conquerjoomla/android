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
#pragma once

#include <Project64-core/Notification.h>

class CNotificationImp :
    public CNotification
{
public:
    CNotificationImp(void);

    void DisplayError(LanguageStringID StringID) const;
    void FatalError(LanguageStringID StringID) const;
    void DisplayMessage(int DisplayTime, LanguageStringID StringID) const;

#ifdef _WIN32
    void DisplayError(const wchar_t * Message) const;
    void FatalError(const wchar_t * Message) const;
    void DisplayMessage(int DisplayTime, const wchar_t * Message) const;
    void DisplayMessage2(const wchar_t * Message) const;

    // Ask a Yes/No Question to the user, yes = true, no = false
    bool AskYesNoQuestion(const wchar_t * Question) const;
#else
	void DisplayError(const char * Message) const;
    void FatalError(const char * Message) const;
    void DisplayMessage(int DisplayTime, const char * Message) const;
#endif
    void BreakPoint(const char * FileName, int32_t LineNumber);

    void AppInitDone(void);
    bool ProcessGuiMessages(void) const;
    void ChangeFullScreen(void) const;
};

CNotificationImp & Notify(void);
