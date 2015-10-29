#include "stdafx.h"
#include "Multilanguage\LanguageSelector.h"

/*bool ChangeDirPermission ( const CPath & Dir)
{
if (Dir.DirectoryExists())
{
HANDLE hDir = CreateFile(Dir,READ_CONTROL|WRITE_DAC,0,NULL,OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS,NULL);
if (hDir != INVALID_HANDLE_VALUE)
{
ACL * pOldDACL = NULL;
PSECURITY_DESCRIPTOR pSD = NULL;

if (GetSecurityInfo(hDir,SE_FILE_OBJECT,DACL_SECURITY_INFORMATION,NULL,NULL,&pOldDACL,NULL,&pSD) == ERROR_SUCCESS)
{
bool bAdd = true;

PEXPLICIT_ACCESS_W pListOfExplictEntries;
ULONG cCountOfExplicitEntries;
if (GetExplicitEntriesFromAclW(pOldDACL,&cCountOfExplicitEntries,&pListOfExplictEntries) == ERROR_SUCCESS)
{
for (int i = 0; i < cCountOfExplicitEntries; i ++)
{
EXPLICIT_ACCESS_W &ea = pListOfExplictEntries[i];
if (ea.grfAccessMode != GRANT_ACCESS) { continue; }
if (ea.grfAccessPermissions != GENERIC_ALL) { continue; }
if ((ea.grfInheritance & (CONTAINER_INHERIT_ACE|OBJECT_INHERIT_ACE)) != (CONTAINER_INHERIT_ACE|OBJECT_INHERIT_ACE)) { continue; }

if (ea.Trustee.TrusteeType == TRUSTEE_IS_SID)
{
}
bAdd = false;
}
}

if (bAdd)
{
EXPLICIT_ACCESS ea = {0};
ea.grfAccessMode = GRANT_ACCESS;
ea.grfAccessPermissions = GENERIC_ALL;
ea.grfInheritance = CONTAINER_INHERIT_ACE|OBJECT_INHERIT_ACE;
ea.Trustee.TrusteeType = TRUSTEE_IS_GROUP;
ea.Trustee.TrusteeForm = TRUSTEE_IS_NAME;
ea.Trustee.ptstrName = TEXT("Users");

ACL * pNewDACL = NULL;
SetEntriesInAcl(1,&ea,pOldDACL,&pNewDACL);

SetSecurityInfo(hDir,SE_FILE_OBJECT,DACL_SECURITY_INFORMATION,NULL,NULL,pNewDACL,NULL);
LocalFree(pNewDACL);
}
LocalFree(pSD);
}
CloseHandle(hDir);
}
}
return true;
}*/

const char * AppName(void)
{
    static stdstr Name;
    if (Name.empty())
    {
        Name = stdstr_f("Project64 %s", VER_FILE_VERSION_STR);
    }
    return Name.c_str();
}

#ifndef WINDOWS_UI
int main(int argc, char* argv[])
{
    while (argc > 0)
    {
        puts(argv[--argc]);
    }
    putchar('\n');

    fprintf(
        stderr,
        "Cross-platform (graphical/terminal?) UI not yet implemented.\n"
        );
    return 0;
}
#else
int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpszArgs*/, int /*nWinMode*/)
{
    try
    {
        AppInit(&Notify());
        if (!g_Lang->IsLanguageLoaded())
        {
            CLanguageSelector().Select();
        }

        //Create the main window with Menu
        WriteTrace(TraceDebug, __FUNCTION__ ": Create Main Window");
        CMainGui  MainWindow(true, stdstr_f("Project64 %s", VER_FILE_VERSION_STR).c_str()), HiddenWindow(false);
        CMainMenu MainMenu(&MainWindow);
#ifdef tofix
        g_Plugins->SetRenderWindows(MainWindow.m_hMainWindow, HiddenWindow.m_hMainWindow);
#endif
        Notify().SetMainWindow(&MainWindow);

        if (__argc > 1)
        {
            WriteTraceF(TraceDebug, __FUNCTION__ ": Cmd line found \"%s\"", __argv[1]);
            MainWindow.Show(true);	//Show the main window
            CN64System::RunFileImage(__argv[1]);
        }
        else
        {
            if (g_Settings->LoadDword(RomBrowser_Enabled))
            {
                WriteTrace(TraceDebug, __FUNCTION__ ": Show Rom Browser");
                //Display the rom browser
                MainWindow.ShowRomList();
                MainWindow.Show(true);	//Show the main window
                MainWindow.HighLightLastRom();
            }
            else {
                WriteTrace(TraceDebug, __FUNCTION__ ": Show Main Window");
                MainWindow.Show(true);	//Show the main window
            }
        }

        //Process Messages till program is closed
        WriteTrace(TraceDebug, __FUNCTION__ ": Entering Message Loop");
        MainWindow.ProcessAllMessages();
        WriteTrace(TraceDebug, __FUNCTION__ ": Message Loop Finished");

        if (g_BaseSystem)
        {
            g_BaseSystem->CloseCpu();
            delete g_BaseSystem;
            g_BaseSystem = NULL;
        }
        WriteTrace(TraceDebug, __FUNCTION__ ": System Closed");
    }
    catch (...)
    {
        WriteTraceF(TraceError, __FUNCTION__ ": Exception caught (File: \"%s\" Line: %d)", __FILE__, __LINE__);
        MessageBox(NULL, stdstr_f("Exception caught\nFile: %s\nLine: %d", __FILE__, __LINE__).c_str(), "Exception", MB_OK);
    }
    AppCleanup();
    return true;
}
#endif