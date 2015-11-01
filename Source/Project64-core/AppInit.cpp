#include "stdafx.h"
#include <windows.h>
#include <objbase.h>
#include <Tlhelp32.h>
#include <common/path.h>
#include <common/trace.h>
#include <Project64-core\N64 System\Mips\Memory Virtual Mem.h>
#include <Project64-core\N64 System\System Globals.h>
#include <Project64-core\Plugins\Plugin Class.h>
#include <Project64-core\N64 System\N64 Rom Class.h>

void FixDirectories(void);
void FixLocale(void);

CNotification * g_Notify = NULL;
static CTraceFileLog * g_LogFile = NULL;

bool TerminatedExistingEmu()
{
	bool bTerminated = false;
	bool AskedUser = false;
	DWORD pid = GetCurrentProcessId();

	HANDLE nSearch = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (nSearch != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 lppe;

		memset(&lppe, 0, sizeof(PROCESSENTRY32));
		lppe.dwSize = sizeof(PROCESSENTRY32);
		stdstr ModuleName = CPath(CPath::MODULE_FILE).GetNameExtension();

		if (Process32First(nSearch, &lppe))
		{
			do
			{
				if (_stricmp(lppe.szExeFile, ModuleName.c_str()) != 0 ||
					lppe.th32ProcessID == pid)
				{
					continue;
				}
				if (!AskedUser)
				{
					AskedUser = true;
					int res = MessageBox(NULL, stdstr_f("Project64.exe currently running\n\nTerminate pid %d now?", lppe.th32ProcessID).c_str(), "Terminate project64", MB_YESNO | MB_ICONEXCLAMATION);
					if (res != IDYES)
					{
						break;
					}
				}
				HANDLE hHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, lppe.th32ProcessID);
				if (hHandle != NULL)
				{
					if (TerminateProcess(hHandle, 0))
					{
						bTerminated = true;
					}
					else
					{
						MessageBox(NULL, stdstr_f("Failed to terminate pid %d", lppe.th32ProcessID).c_str(), "Terminate project64 failed!", MB_YESNO | MB_ICONEXCLAMATION);
					}
					CloseHandle(hHandle);
				}
			} while (Process32Next(nSearch, &lppe));
		}
		CloseHandle(nSearch);
	}
	return bTerminated;
}

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
	g_LogFile->SetTraceLevel((TraceLevel)g_Settings->LoadDword(Debugger_AppLogLevel));
	AddTraceModule(g_LogFile);

	g_Settings->RegisterChangeCB(Debugger_AppLogLevel, g_LogFile, (CSettings::SettingChangedFunc)LogLevelChanged);
	g_Settings->RegisterChangeCB(Debugger_AppLogFlush, g_LogFile, (CSettings::SettingChangedFunc)LogFlushChanged);
}

void AppInit(CNotification * Notify)
{
	try
	{
		g_Notify = Notify;

		FixDirectories();
		FixLocale();

		CoInitialize(NULL);

		stdstr_f AppName("Project64 %s", VER_FILE_VERSION_STR);

		SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

		g_Settings = new CSettings;
		g_Settings->Initialize(AppName.c_str());

		if (g_Settings->LoadBool(Setting_CheckEmuRunning) &&
			TerminatedExistingEmu())
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
	}
	catch (...)
	{
		g_Notify->DisplayError(stdstr_f("Exception caught\nFile: %s\nLine: %d", __FILE__, __LINE__).ToUTF16().c_str());
	}
}

void AppCleanup(void)
{
	g_Settings->UnregisterChangeCB(Debugger_AppLogLevel, g_LogFile, (CSettings::SettingChangedFunc)LogLevelChanged);
	g_Settings->UnregisterChangeCB(Debugger_AppLogFlush, g_LogFile, (CSettings::SettingChangedFunc)LogFlushChanged);
	WriteTrace(TraceDebug, __FUNCTION__ ": cleaning up global objects");

	if (g_Rom)      { delete g_Rom; g_Rom = NULL; }
	if (g_Plugins)  { delete g_Plugins; g_Plugins = NULL; }
	if (g_Settings) { delete g_Settings; g_Settings = NULL; }
	if (g_Lang)     { delete g_Lang; g_Lang = NULL; }

	CMipsMemoryVM::FreeReservedMemory();

	CoUninitialize();
	WriteTrace(TraceDebug, __FUNCTION__ ": Done");
	CloseTrace();
}

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

void FixLocale(void)
{
	char *lbuffer = new char[10];
	if (GetLocaleInfoA(LOCALE_SYSTEM_DEFAULT, LOCALE_SABBREVLANGNAME, lbuffer, 10))
	{
		setlocale(LC_ALL, lbuffer);
	}
	delete lbuffer;
}