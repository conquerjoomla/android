#include "stdafx.h"
#include <windows.h>
#include <objbase.h>

void FixDirectories ( void );
void FixLocale ( void );

CNotification * g_Notify = NULL;

void AppInit ( CNotification * Notify )
{
    try
    {
        g_Notify = Notify;

        FixDirectories();
        FixLocale();

        CoInitialize(NULL);

        stdstr_f AppName("Project64 %s", VER_FILE_VERSION_STR);

        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL );
        g_Lang = new CLanguage();

        g_Settings = new CSettings;
        g_Settings->Initialize(AppName.c_str());

#ifdef tofix
        if (g_Settings->LoadBool(Setting_CheckEmuRunning) &&
            TerminatedExistingEmu())
        {
            delete g_Settings;
            g_Settings = new CSettings;
            g_Settings->Initialize(AppName.c_str());
        }

        InitializeLog();

        WriteTrace(TraceDebug,__FUNCTION__ ": Application Starting");
        CMipsMemoryVM::ReserveMemory();

        //Create the plugin container
        WriteTrace(TraceDebug,__FUNCTION__ ": Create Plugins");
        g_Plugins = new CPlugins(g_Settings->LoadString(Directory_Plugin));
#endif

        //Select the language
        g_Lang->LoadCurrentStrings(true);
    }
    catch(...)
    {
#ifdef tofix
        WriteTraceF(TraceError,__FUNCTION__ ": Exception caught (File: \"%s\" Line: %d)",__FILE__,__LINE__);
        MessageBox(NULL,stdstr_f("Exception caught\nFile: %s\nLine: %d",__FILE__,__LINE__).c_str(),"Exception",MB_OK);
#endif
    }
}

void AppCleanup ( void )
{
#ifdef tofix
    WriteTrace(TraceDebug,__FUNCTION__ ": cleaning up global objects");

    if (g_Rom)      { delete g_Rom; g_Rom = NULL; }
    if (g_Plugins)  { delete g_Plugins; g_Plugins = NULL; }
    if (g_Settings) { delete g_Settings; g_Settings = NULL; }
    if (g_Lang)     { delete g_Lang; g_Lang = NULL; }

    CMipsMemoryVM::FreeReservedMemory();

    CoUninitialize();
    WriteTrace(TraceDebug,__FUNCTION__ ": Done");
    CloseTrace();
#endif
}

void FixDirectories ( void )
{
#ifdef tofix
    CPath Directory(CPath::MODULE_DIRECTORY);
    Directory.AppendDirectory("Config");
    if (!Directory.DirectoryExists()) Directory.CreateDirectory();

    Directory.UpDirectory();
    Directory.AppendDirectory("Logs");
    if (!Directory.DirectoryExists()) Directory.CreateDirectory();

    Directory.UpDirectory();
    Directory.AppendDirectory("Save");
    if (!Directory.DirectoryExists()) Directory.CreateDirectory();

    Directory.UpDirectory();
    Directory.AppendDirectory("Screenshots");
    if (!Directory.DirectoryExists()) Directory.CreateDirectory();

    Directory.UpDirectory();
    Directory.AppendDirectory("textures");
    if (!Directory.DirectoryExists()) Directory.CreateDirectory();
#endif
}

void FixLocale ( void )
{
    char *lbuffer = new char[10];
    if (GetLocaleInfoA(LOCALE_SYSTEM_DEFAULT, LOCALE_SABBREVLANGNAME, lbuffer, 10))
        setlocale(LC_ALL, lbuffer);
    delete lbuffer;
}