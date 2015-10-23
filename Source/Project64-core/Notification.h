#pragma once

__interface CNotification
{
public:
    //Error Messages
    virtual void DisplayError     ( const wchar_t * Message ) const = 0;
    virtual void DisplayError     ( LanguageStringID StringID ) const = 0;

    virtual void FatalError       ( const wchar_t * Message ) const = 0;
    virtual void FatalError       ( LanguageStringID StringID ) const = 0;

    //User Feedback
    virtual void DisplayMessage   ( int DisplayTime, const wchar_t * Message ) const = 0;
    virtual void DisplayMessage   ( int DisplayTime, LanguageStringID StringID ) const = 0;

    virtual void DisplayMessage2  ( const wchar_t * Message ) const = 0;

    virtual void BreakPoint       ( const wchar_t * FileName, const int LineNumber) = 0;
};

extern CNotification * g_Notify;
