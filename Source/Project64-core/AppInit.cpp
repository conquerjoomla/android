#include "stdafx.h"
#ifdef tofix
#include <common\path.h>
#include <common\trace.h>
#include <Common\Util.h>
#include <Project64-core/N64System/Mips/MemoryVirtualMem.h>
#include <Project64-core/N64System/SystemGlobals.h>
#include <Project64-core/Plugins/PluginClass.h>
#include <Project64-core/N64System/N64RomClass.h>

void FixDirectories(void);
void FixLocale(void);

static void IncreaseThreadPriority(void);
#endif
CNotification * g_Notify = NULL;
#ifdef tofix
static CTraceFileLog * g_LogFile = NULL;

void LogLevelChanged(CTraceFileLog * LogFile)
{
    LogFile->SetTraceLevel((TraceLevel)g_Settings->LoadDword(Debugger_AppLogLevel));
}

void LogFlushChanged(CTraceFileLog * LogFile)
{
    LogFile->SetFlushFile(g_Settings->LoadDword(Debugger_AppLogFlush) != 0);
}

void InitializeLog(void)
{
    CPath LogFilePath(CPath::MODULE_DIRECTORY);
    LogFilePath.AppendDirectory("Logs");
    if (!LogFilePath.DirectoryExists())
    {
        LogFilePath.DirectoryCreate();
    }
    LogFilePath.SetNameExtension("Project64.log");

    g_LogFile = new CTraceFileLog(LogFilePath, g_Settings->LoadDword(Debugger_AppLogFlush) != 0, Log_New, 500);
#ifdef VALIDATE_DEBUG
    g_LogFile->SetTraceLevel((TraceLevel)(g_Settings->LoadDword(Debugger_AppLogLevel) | TraceValidate | TraceDebug));
#else
    g_LogFile->SetTraceLevel((TraceLevel)g_Settings->LoadDword(Debugger_AppLogLevel));
#endif
    AddTraceModule(g_LogFile);

    g_Settings->RegisterChangeCB(Debugger_AppLogLevel, g_LogFile, (CSettings::SettingChangedFunc)LogLevelChanged);
    g_Settings->RegisterChangeCB(Debugger_AppLogFlush, g_LogFile, (CSettings::SettingChangedFunc)LogFlushChanged);
}
#endif

#if defined(ANDROID)
#include <android/log.h>

#define printf(...) __android_log_print(ANDROID_LOG_VERBOSE, "UI-Console", __VA_ARGS__)
#endif

void AppInit(CNotification * Notify)
{
    try
    {
#ifdef tofix
        g_Notify = Notify;
#endif
		stdstr_f AppName("Project64 %s", VER_FILE_VERSION_STR);
        g_Settings = new CSettings;
        g_Settings->Initialize(AppName.c_str());

		printf("SupportFile_Settings = %s\n",g_Settings->LoadStringVal(SupportFile_Settings).c_str());
        //Parse Command line

		printf("SupportFile_Settings = %s\n",g_Settings->LoadStringVal(SupportFile_Settings).c_str());
#ifdef tofix
        FixDirectories();
        FixLocale();
        IncreaseThreadPriority();

        if (g_Settings->LoadBool(Setting_CheckEmuRunning) &&
            pjutil::TerminatedExistingExe())
        {
            delete g_Settings;
            g_Settings = new CSettings;
            g_Settings->Initialize(AppName.c_str());
        }

        InitializeLog();

        WriteTrace(TraceDebug, __FUNCTION__ ": Application Starting");
        CMipsMemoryVM::ReserveMemory();

        //Create the plugin container
        WriteTrace(TraceDebug, __FUNCTION__ ": Create Plugins");
        g_Plugins = new CPlugins(g_Settings->LoadStringVal(Directory_Plugin));

        g_Lang = new CLanguage();
        g_Lang->LoadCurrentStrings();
        g_Notify->AppInitDone();
#endif
    }
    catch (...)
    {
#ifdef tofix
        g_Notify->DisplayError(stdstr_f("Exception caught\nFile: %s\nLine: %d", __FILE__, __LINE__).ToUTF16().c_str());
#endif
    }
}

void AppCleanup(void)
{
#ifdef tofix
    g_Settings->UnregisterChangeCB(Debugger_AppLogLevel, g_LogFile, (CSettings::SettingChangedFunc)LogLevelChanged);
    g_Settings->UnregisterChangeCB(Debugger_AppLogFlush, g_LogFile, (CSettings::SettingChangedFunc)LogFlushChanged);
    WriteTrace(TraceDebug, __FUNCTION__ ": cleaning up global objects");

    if (g_Rom)      { delete g_Rom; g_Rom = NULL; }
    if (g_Plugins)  { delete g_Plugins; g_Plugins = NULL; }
    if (g_Lang)     { delete g_Lang; g_Lang = NULL; }
#endif
    if (g_Settings) { delete g_Settings; g_Settings = NULL; }

#ifdef tofix
    CMipsMemoryVM::FreeReservedMemory();
    WriteTrace(TraceDebug, __FUNCTION__ ": Done");
    CloseTrace();
#endif
}

#ifdef tofix
void FixDirectories(void)
{
    CPath Directory(CPath::MODULE_DIRECTORY);
    Directory.AppendDirectory("Config");
    if (!Directory.DirectoryExists()) Directory.DirectoryCreate();

    Directory.UpDirectory();
    Directory.AppendDirectory("Logs");
    if (!Directory.DirectoryExists()) Directory.DirectoryCreate();

    Directory.UpDirectory();
    Directory.AppendDirectory("Save");
    if (!Directory.DirectoryExists()) Directory.DirectoryCreate();

    Directory.UpDirectory();
    Directory.AppendDirectory("Screenshots");
    if (!Directory.DirectoryExists()) Directory.DirectoryCreate();

    Directory.UpDirectory();
    Directory.AppendDirectory("textures");
    if (!Directory.DirectoryExists()) Directory.DirectoryCreate();
}

#include <windows.h>
void FixLocale(void)
{
    char *lbuffer = new char[10];
    if (GetLocaleInfoA(LOCALE_SYSTEM_DEFAULT, LOCALE_SABBREVLANGNAME, lbuffer, 10))
    {
        setlocale(LC_ALL, lbuffer);
    }
    delete[] lbuffer;
}

void IncreaseThreadPriority(void)
{
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
}
#endif