#pragma once

#include <Project64-core\Settings\Debug Settings.h>

enum MainMenuID
{
    //File Menu
    ID_FILE_OPEN_ROM = 4000, ID_FILE_ROM_INFO, ID_FILE_STARTEMULATION, ID_FILE_ENDEMULATION,
    ID_FILE_ROMDIRECTORY, ID_FILE_REFRESHROMLIST, ID_FILE_EXIT,

    //language
    ID_LANG_START, ID_LANG_END = ID_LANG_START + 100,

    //Recent Files
    ID_RECENT_ROM_START, ID_RECENT_ROM_END = ID_RECENT_ROM_START + 20,

    //Recent Dir
    ID_RECENT_DIR_START, ID_RECENT_DIR_END = ID_RECENT_DIR_START + 20,

    //System Menu
    ID_SYSTEM_RESET_SOFT, ID_SYSTEM_RESET_HARD, ID_SYSTEM_PAUSE, ID_SYSTEM_BITMAP,
    ID_SYSTEM_LIMITFPS, ID_SYSTEM_RESTORE, ID_SYSTEM_LOAD, ID_SYSTEM_SAVE,
    ID_SYSTEM_SAVEAS, ID_SYSTEM_CHEAT, ID_SYSTEM_GSBUTTON,

    //Current Save Slot
    ID_CURRENT_SAVE_1, ID_CURRENT_SAVE_2, ID_CURRENT_SAVE_3, ID_CURRENT_SAVE_4, ID_CURRENT_SAVE_5,
    ID_CURRENT_SAVE_6, ID_CURRENT_SAVE_7, ID_CURRENT_SAVE_8, ID_CURRENT_SAVE_9, ID_CURRENT_SAVE_10,
    ID_CURRENT_SAVE_DEFAULT,

    //Option Menu
    ID_OPTIONS_FULLSCREEN, ID_OPTIONS_FULLSCREEN2, ID_OPTIONS_ALWAYSONTOP, ID_OPTIONS_CONFIG_GFX,
    ID_OPTIONS_CONFIG_AUDIO, ID_OPTIONS_CONFIG_CONT, ID_OPTIONS_CONFIG_RSP, ID_OPTIONS_CPU_USAGE,
    ID_OPTIONS_SETTINGS, ID_OPTIONS_DISPLAY_FR, ID_OPTIONS_CHANGE_FR, ID_OPTIONS_INCREASE_SPEED,
    ID_OPTIONS_DECREASE_SPEED,

    //Debugger Menu
    ID_DEBUG_SHOW_TLB_MISSES, ID_DEBUG_SHOW_UNHANDLED_MEM, ID_DEBUG_SHOW_PIF_ERRORS,
    ID_DEBUG_SHOW_DLIST_COUNT, ID_DEBUG_SHOW_RECOMP_MEM_SIZE, ID_DEBUG_SHOW_DIV_BY_ZERO,
    ID_DEBUG_GENERATE_LOG_FILES, ID_DEBUG_DISABLE_GAMEFIX,
    ID_DEBUGGER_LOGOPTIONS, ID_DEBUGGER_GENERATELOG, ID_DEBUGGER_DUMPMEMORY, ID_DEBUGGER_SEARCHMEMORY,
    ID_DEBUGGER_TLBENTRIES, ID_DEBUGGER_BREAKPOINTS, ID_DEBUGGER_MEMORY, ID_DEBUGGER_R4300REGISTERS,
    ID_DEBUGGER_INTERRUPT_SP, ID_DEBUGGER_INTERRUPT_SI, ID_DEBUGGER_INTERRUPT_AI, ID_DEBUGGER_INTERRUPT_VI,
    ID_DEBUGGER_INTERRUPT_PI, ID_DEBUGGER_INTERRUPT_DP,

    // App logging
    ID_DEBUGGER_APPLOG_ERRORS, ID_DEBUGGER_APPLOG_SETTINGS, ID_DEBUGGER_APPLOG_GFX_PLUGIN,
    ID_DEBUGGER_APPLOG_DEBUG, ID_DEBUGGER_APPLOG_FLUSH, ID_DEBUGGER_APPLOG_RECOMPILER,
    ID_DEBUGGER_APPLOG_RSP, ID_DEBUGGER_APPLOG_TLB, ID_DEBUGGER_APPLOG_AUDIO_EMU,

    //Profile Menu
    ID_PROFILE_PROFILE, ID_PROFILE_RESETCOUNTER, ID_PROFILE_GENERATELOG,

    //Help Menu
    ID_HELP_SUPPORTFORUM, ID_HELP_HOMEPAGE, ID_HELP_ABOUTSETTINGFILES, ID_HELP_ABOUT,
};

class CMainMenu :
    public CBaseMenu,
    private CDebugSettings
{
public:
    CMainMenu(CMainGui * Window);
    ~CMainMenu();

    int       ProcessAccelerator(HWND hWnd, void * lpMsg);
    bool      ProcessMessage(HWND hWnd, DWORD wNotifyCode, DWORD wID);
    void      ResetMenu(void);
    void      ResetAccelerators(void) { m_ResetAccelerators = true; }

private:
    CMainMenu();								// Disable default constructor
    CMainMenu(const CMainMenu&);				// Disable copy constructor
    CMainMenu& operator=(const CMainMenu&);		// Disable assignment

    void FillOutMenu(HMENU hMenu);
    std::wstring GetSaveSlotString(int Slot);
    stdstr GetFileLastMod(stdstr FileName);
    void RebuildAccelerators(void);
    stdstr ChooseFileToOpen(HWND hParent);

    static void SettingsChanged(CMainMenu * _this);

    typedef std::list<SettingID> SettingList;

    CMainGui   * m_Gui;

    void *       m_AccelTable;
    bool         m_ResetAccelerators;
    CShortCuts   m_ShortCuts;
    SettingList  m_ChangeSettingList;
    CriticalSection m_CS;
};
