#pragma once

#include "Multilanguage.h"

#ifndef _MSC_VER
#define __interface struct
#endif

__interface CNotification
{
public:
    virtual void DisplayError(LanguageStringID StringID) const = 0;
    virtual void FatalError(LanguageStringID StringID) const = 0;
    virtual void DisplayMessage(int DisplayTime, LanguageStringID StringID) const = 0;

#ifdef _WIN32
    virtual void DisplayError(const wchar_t * Message) const = 0;
    virtual void FatalError(const wchar_t * Message) const = 0;
    virtual void DisplayMessage(int DisplayTime, const wchar_t * Message) const = 0;
    virtual void DisplayMessage2(const wchar_t * Message) const = 0;

    // Ask a Yes/No Question to the user, yes = true, no = false
    virtual bool AskYesNoQuestion(const wchar_t * Question) const = 0;
#else
    virtual void DisplayError(const char * Message) const = 0;
    virtual void FatalError(const char * Message) const = 0;
    virtual void DisplayMessage(int DisplayTime, const char * Message) const = 0;
#endif
    virtual void BreakPoint(const char * FileName, int32_t LineNumber) = 0;

    virtual void AppInitDone(void) = 0;
    virtual bool ProcessGuiMessages(void) const = 0;
    virtual void ChangeFullScreen(void) const = 0;
};

extern CNotification * g_Notify;
