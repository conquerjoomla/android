#include "stdafx.h"
#include <Common/path.h>
#include <Common/trace.h>
#include <Common/Util.h>
#include <Project64-core/N64System/Mips/MemoryVirtualMem.h>
#ifdef tofix
#include <Project64-core/N64System/SystemGlobals.h>
#include <Project64-core/Plugins/PluginClass.h>
#include <Project64-core/N64System/N64RomClass.h>
#endif
#include "Settings/SettingType/SettingsType-Application.h"

#if defined(ANDROID)
#include <android/log.h>

#define printf(...) __android_log_print(ANDROID_LOG_VERBOSE, "UI-Console", __VA_ARGS__)
#endif

static void FixDirectories(void);

#ifdef _WIN32
static void IncreaseThreadPriority(void);
#endif

CNotification * g_Notify = NULL;
static CTraceFileLog * g_LogFile = NULL;

void LogFlushChanged(CTraceFileLog * LogFile)
{
    LogFile->SetFlushFile(g_Settings->LoadDword(Debugger_AppLogFlush) != 0);
}

void SetTraceModuleNames(void)
{
    TraceSetModuleName(TraceMD5, "MD5");
    TraceSetModuleName(TraceSettings, "Settings");
    TraceSetModuleName(TraceUnknown, "Unknown");
    TraceSetModuleName(TraceAppInit, "App Init");
    TraceSetModuleName(TraceAppCleanup, "App Cleanup");
    TraceSetModuleName(TraceN64System, "N64 System");
    TraceSetModuleName(TracePlugins, "Plugins");
    TraceSetModuleName(TraceGFXPlugin, "GFX Plugin");
    TraceSetModuleName(TraceAudioPlugin, "Audio Plugin");
    TraceSetModuleName(TraceControllerPlugin, "Controller Plugin");
    TraceSetModuleName(TraceRSPPlugin, "RSP Plugin");
    TraceSetModuleName(TraceRSP, "RSP");
    TraceSetModuleName(TraceAudio, "Audio");
    TraceSetModuleName(TraceRegisterCache, "Register Cache");
    TraceSetModuleName(TraceRecompiler, "Recompiler");
    TraceSetModuleName(TraceTLB, "TLB");
    TraceSetModuleName(TraceProtectedMem, "Protected Memory");
    TraceSetModuleName(TraceUserInterface, "User Interface");
}

void InitializeLog(void)
{
#ifdef ANDROID
    TraceSetMaxModule(MaxTraceModuleProject64, TraceVerbose);
#else
#ifdef _DEBUG
    TraceSetMaxModule(MaxTraceModuleProject64, TraceInfo);
#else
    TraceSetMaxModule(MaxTraceModuleProject64, TraceError);
#endif
#endif
    SetTraceModuleNames();
}

void AddLogModule(void)
{
	CPath LogFilePath(g_Settings->LoadStringVal(Cmd_BaseDirectory).c_str(),"");
    LogFilePath.AppendDirectory("Logs");
    if (!LogFilePath.DirectoryExists())
    {
        LogFilePath.DirectoryCreate();
    }
    LogFilePath.SetNameExtension("Project64.log");

    g_LogFile = new CTraceFileLog(LogFilePath, g_Settings->LoadDword(Debugger_AppLogFlush) != 0, Log_New, 500);
    TraceAddModule(g_LogFile);
}

void UpdateTraceLevel(void * /*NotUsed*/)
{
    g_ModuleLogLevel[TraceMD5] = (uint8_t)g_Settings->LoadDword(Debugger_TraceMD5);
    g_ModuleLogLevel[TraceSettings] = (uint8_t)g_Settings->LoadDword(Debugger_TraceSettings);
    g_ModuleLogLevel[TraceUnknown] = (uint8_t)g_Settings->LoadDword(Debugger_TraceUnknown);
    g_ModuleLogLevel[TraceAppInit] = (uint8_t)g_Settings->LoadDword(Debugger_TraceAppInit);
    g_ModuleLogLevel[TraceAppCleanup] = (uint8_t)g_Settings->LoadDword(Debugger_TraceAppCleanup);
    g_ModuleLogLevel[TraceN64System] = (uint8_t)g_Settings->LoadDword(Debugger_TraceN64System);
    g_ModuleLogLevel[TracePlugins] = (uint8_t)g_Settings->LoadDword(Debugger_TracePlugins);
    g_ModuleLogLevel[TraceGFXPlugin] = (uint8_t)g_Settings->LoadDword(Debugger_TraceGFXPlugin);
    g_ModuleLogLevel[TraceAudioPlugin] = (uint8_t)g_Settings->LoadDword(Debugger_TraceAudioPlugin);
    g_ModuleLogLevel[TraceControllerPlugin] = (uint8_t)g_Settings->LoadDword(Debugger_TraceControllerPlugin);
    g_ModuleLogLevel[TraceRSPPlugin] = (uint8_t)g_Settings->LoadDword(Debugger_TraceRSPPlugin);
    g_ModuleLogLevel[TraceRSP] = (uint8_t)g_Settings->LoadDword(Debugger_TraceRSP);
    g_ModuleLogLevel[TraceAudio] = (uint8_t)g_Settings->LoadDword(Debugger_TraceAudio);
    g_ModuleLogLevel[TraceRegisterCache] = (uint8_t)g_Settings->LoadDword(Debugger_TraceRegisterCache);
    g_ModuleLogLevel[TraceRecompiler] = (uint8_t)g_Settings->LoadDword(Debugger_TraceRecompiler);
    g_ModuleLogLevel[TraceTLB] = (uint8_t)g_Settings->LoadDword(Debugger_TraceTLB);
    g_ModuleLogLevel[TraceProtectedMem] = (uint8_t)g_Settings->LoadDword(Debugger_TraceProtectedMEM);
    g_ModuleLogLevel[TraceUserInterface] = (uint8_t)g_Settings->LoadDword(Debugger_TraceUserInterface);
}

void SetupTrace(void)
{
    AddLogModule();

    g_Settings->RegisterChangeCB(Debugger_TraceMD5, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->RegisterChangeCB(Debugger_TraceSettings, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->RegisterChangeCB(Debugger_TraceUnknown, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->RegisterChangeCB(Debugger_TraceAppInit, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->RegisterChangeCB(Debugger_TraceAppCleanup, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->RegisterChangeCB(Debugger_TraceN64System, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->RegisterChangeCB(Debugger_TracePlugins, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->RegisterChangeCB(Debugger_TraceGFXPlugin, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->RegisterChangeCB(Debugger_TraceAudioPlugin, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->RegisterChangeCB(Debugger_TraceControllerPlugin, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->RegisterChangeCB(Debugger_TraceRSPPlugin, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->RegisterChangeCB(Debugger_TraceRSP, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->RegisterChangeCB(Debugger_TraceAudio, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->RegisterChangeCB(Debugger_TraceRegisterCache, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->RegisterChangeCB(Debugger_TraceRecompiler, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->RegisterChangeCB(Debugger_TraceTLB, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->RegisterChangeCB(Debugger_TraceProtectedMEM, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->RegisterChangeCB(Debugger_TraceUserInterface, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->RegisterChangeCB(Debugger_AppLogFlush, g_LogFile, (CSettings::SettingChangedFunc)LogFlushChanged);
    printf("SetupTrace: 4\n");
    UpdateTraceLevel(NULL);

    printf("SetupTrace: 5\n");
    WriteTrace(TraceAppInit, TraceInfo, "Application Starting %s", VER_FILE_VERSION_STR);
    printf("SetupTrace: 6\n");
}

void CleanupTrace(void)
{
    WriteTrace(TraceAppCleanup, TraceDebug, "Done");

    g_Settings->UnregisterChangeCB(Debugger_TraceMD5, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->UnregisterChangeCB(Debugger_TraceSettings, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->UnregisterChangeCB(Debugger_TraceUnknown, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->UnregisterChangeCB(Debugger_TraceAppInit, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->UnregisterChangeCB(Debugger_TraceAppCleanup, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->UnregisterChangeCB(Debugger_TraceN64System, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->UnregisterChangeCB(Debugger_TracePlugins, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->UnregisterChangeCB(Debugger_TraceGFXPlugin, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->UnregisterChangeCB(Debugger_TraceAudioPlugin, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->UnregisterChangeCB(Debugger_TraceControllerPlugin, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->UnregisterChangeCB(Debugger_TraceRSPPlugin, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->UnregisterChangeCB(Debugger_TraceRSP, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->UnregisterChangeCB(Debugger_TraceAudio, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->UnregisterChangeCB(Debugger_TraceRegisterCache, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->UnregisterChangeCB(Debugger_TraceRecompiler, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->UnregisterChangeCB(Debugger_TraceTLB, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->UnregisterChangeCB(Debugger_TraceProtectedMEM, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->UnregisterChangeCB(Debugger_TraceUserInterface, NULL, (CSettings::SettingChangedFunc)UpdateTraceLevel);
    g_Settings->UnregisterChangeCB(Debugger_AppLogFlush, g_LogFile, (CSettings::SettingChangedFunc)LogFlushChanged);

    CloseTrace();
    if (g_LogFile) { delete g_LogFile; g_LogFile = NULL; }
}

const char * AppName ( void )
{
	static stdstr_f ApplicationName("Project64 %s", VER_FILE_VERSION_STR);
	return ApplicationName.c_str();
}

static bool ParseCommand(int32_t argc, char **argv)
{
    if (argc == 1)
    {
        return true;
    }
    for (int32_t i = 1; i < argc; i++)
    {
        int32_t ArgsLeft = argc - i - 1;
        printf("ParseCommand: %d=%s\n",i,argv[i]);
        if (strcmp(argv[i], "--basedir") == 0 && ArgsLeft >= 1)
        {
            printf("setting Cmd_BaseDirectory to %s", argv[i + 1]);
            g_Settings->SaveString(Cmd_BaseDirectory, argv[i + 1]);
			CSettingTypeApplication::Initialize(AppName());
            printf("Reinit done\n");
			i++;
        }
        else if (strcmp(argv[i], "--help") == 0)
        {
            g_Settings->SaveBool(Cmd_ShowHelp, true);
            return false;
        }
        else if (ArgsLeft == 0 && argv[i][0] != '-')
        {
            printf("before save Cmd_RomFile\n");
            g_Settings->SaveString(Cmd_RomFile, &(argv[i][0]));
            printf("after save Cmd_RomFile\n");
            return true;
        }
        else
        {
            //WriteTraceF(TraceError, __FUNCTION__ ": unrecognized command-line parameter '%s'", argv[i]);
        }
    }
    return false;
}

bool AppInit(CNotification * Notify, int argc, char **argv)
{
    try
    {
        g_Notify = Notify;
        InitializeLog();
		if (Notify == NULL)
		{
			WriteTrace(TraceAppInit, TraceError, "No Notification class passed");
			return false;
		}
        g_Settings = new CSettings;
        printf("Created Settings, going to init now\n");
        g_Settings->Initialize(AppName());
        printf("g_Settings->Initialize done\n");

        printf("SupportFile_Settings = %s\n", g_Settings->LoadStringVal(SupportFile_Settings).c_str());
        if (!ParseCommand(argc, argv))
        {
            return false;
        }

        printf("SupportFile_Settings = %s\n", g_Settings->LoadStringVal(SupportFile_Settings).c_str());
#ifdef _WIN32
        if (g_Settings->LoadBool(Setting_CheckEmuRunning) &&
            pjutil::TerminatedExistingExe())
        {
            delete g_Settings;
            g_Settings = new CSettings;
            g_Settings->Initialize(AppName());
        }
#endif

        printf("calling SetupTrace\n");
        SetupTrace();
        printf("SetupTrace Done\n");
		FixDirectories();
        printf("FixDirectories Done\n");
#ifdef _WIN32
        CMipsMemoryVM::ReserveMemory();
        IncreaseThreadPriority();
#endif

        //Create the plugin container
        printf("calling WriteTrace\n");
        WriteTrace(TraceAppInit, TraceInfo, "Create Plugins");
        printf("WriteTrace done\n");
#ifdef tofix
		g_Plugins = new CPlugins(g_Settings->LoadStringVal(Directory_Plugin));
        g_Lang = new CLanguage();
        g_Lang->LoadCurrentStrings();
#endif
        printf("calling AppInitDone\n");
        g_Notify->AppInitDone();
        printf("AppInitDone done\n");
    }
    catch (...)
    {
        g_Notify->DisplayError(stdstr_f("Exception caught\nFile: %s\nLine: %d", __FILE__, __LINE__).ToUTF16().c_str());
    }
    return true;
}

void AppCleanup(void)
{
    WriteTrace(TraceAppCleanup, TraceDebug, "cleaning up global objects");

#ifdef tofix
    if (g_Rom)      { delete g_Rom; g_Rom = NULL; }
    if (g_Plugins)  { delete g_Plugins; g_Plugins = NULL; }
#endif
    if (g_Settings) { delete g_Settings; g_Settings = NULL; }
#ifdef tofix
    if (g_Lang)     { delete g_Lang; g_Lang = NULL; }
#endif

#ifdef tofix
    CMipsMemoryVM::FreeReservedMemory();
#endif
    CleanupTrace();
}

void FixDirectories(void)
{
	CPath Directory(g_Settings->LoadStringVal(Cmd_BaseDirectory).c_str(),"");
    Directory.AppendDirectory("Save");
    if (!Directory.DirectoryExists()) Directory.DirectoryCreate();

    Directory.UpDirectory();
    Directory.AppendDirectory("Screenshots");
    if (!Directory.DirectoryExists()) Directory.DirectoryCreate();

    Directory.UpDirectory();
    Directory.AppendDirectory("textures");
    if (!Directory.DirectoryExists()) Directory.DirectoryCreate();
}

#ifdef _WIN32
#include <windows.h>
void IncreaseThreadPriority(void)
{
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
}
#endif