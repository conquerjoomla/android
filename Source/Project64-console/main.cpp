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
#include "SDL_main.h"
#include "NotificationClass.h"
#include <Project64-core/AppInit.h>
#include <Project64-core/Settings/SettingsClass.h>
#include <Project64-core/N64System/N64Class.h>
#include <Common/Trace.h>

#ifdef ANDROID
#include <android/log.h>

class AndroidLogger : public CTraceModule
{
    void Write(uint32_t module, uint8_t severity, const char * file, int line, const char * function, const char * Message)
    {
        switch (severity)
        {
        case TraceError: __android_log_print(ANDROID_LOG_ERROR, TraceModule(module), "%s: %s",function,Message); break;
        case TraceWarning:  __android_log_print(ANDROID_LOG_WARN, TraceModule(module), "%s: %s",function,Message); break;
        case TraceNotice: __android_log_print(ANDROID_LOG_INFO, TraceModule(module), "%s: %s",function,Message); break;
        case TraceInfo: __android_log_print(ANDROID_LOG_INFO, TraceModule(module), "%s: %s",function,Message); break;
        case TraceDebug: __android_log_print(ANDROID_LOG_DEBUG, TraceModule(module), "%s: %s",function,Message); break;
        case TraceVerbose: __android_log_print(ANDROID_LOG_VERBOSE, TraceModule(module), "%s: %s",function,Message); break;
        default: __android_log_print(ANDROID_LOG_UNKNOWN, TraceModule(module), "%s: %s",function,Message); break;
        }
    }
};
#endif

#ifndef _WIN32
/* Allow external modules to call the main function as a library method.  This is useful for user
* interfaces that simply layer on top of (rather than re-implement) UI-Console (e.g. for android).
*/
__attribute__((visibility("default")))
#endif
    int main(int argc, char * argv[])
{
#ifdef ANDROID
    AndroidLogger Logger;
    TraceAddModule(&Logger);
#endif
    Notify().DisplayMessage(10,"    ____               _           __  _____ __ __");
    Notify().DisplayMessage(10,"   / __ \\_________    (_)__  _____/ /_/ ___// // /");
    Notify().DisplayMessage(10,"  / /_/ / ___/ __ \\  / / _ \\/ ___/ __/ __ \\/ // /_");
    Notify().DisplayMessage(10," / ____/ /  / /_/ / / /  __/ /__/ /_/ /_/ /__  __/");
    Notify().DisplayMessage(10,"/_/   /_/   \\____/_/ /\\___/\\___/\\__/\\____/  /_/");
    Notify().DisplayMessage(10,"                /___/");
    Notify().DisplayMessage(10,"http://www.pj64-emu.com/");
    Notify().DisplayMessage(10,stdstr_f("%s Version %s", VER_FILE_DESCRIPTION_STR, VER_FILE_VERSION_STR).c_str());
    Notify().DisplayMessage(10,"");

    if (!AppInit(&Notify(), argc, &argv[0]))
    {
        AppCleanup();
        return -1;
    }

#ifdef _WIN32
#ifdef tofix
    //Create the main window with Menu
    WriteTrace(TraceUserInterface, TraceDebug, "Create Main Window");
    CMainGui  MainWindow(true, stdstr_f("Project64 %s", VER_FILE_VERSION_STR).c_str()), HiddenWindow(false);
    g_Plugins->SetRenderWindows(&MainWindow, &HiddenWindow);
    Notify().SetMainWindow(&MainWindow);
#endif
#endif

    if (g_Settings->LoadStringVal(Cmd_RomFile).length() > 0)
    {
#ifdef _WIN32
        CN64System::RunFileImage(g_Settings->LoadStringVal(Cmd_RomFile).c_str(), true);
#else
        CN64System::RunFileImage(g_Settings->LoadStringVal(Cmd_RomFile).c_str(), false);
#endif
    }
#ifdef _WIN32
    //run message loop
#endif
    AppCleanup();
    return 0;
}