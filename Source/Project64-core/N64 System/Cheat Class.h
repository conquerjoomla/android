/****************************************************************************
*                                                                           *
* Project 64 - A Nintendo 64 emulator.                                      *
* http://www.pj64-emu.com/                                                  *
* Copyright (C) 2012 Project64. All rights reserved.                        *
*                                                                           *
* License:                                                                  *
* GNU/GPLv2 http://www.gnu.org/licenses/gpl-2.0.html                        *
*                                                                           *
****************************************************************************/
#pragma once
#include "N64 Rom Class.h"
#include <Project64-core\N64 System\Mips\Memory Class.h>
#include <Project64-core\Plugins\Plugin Class.h>

class CCheats
{
public:
    CCheats(const CN64Rom * Rom = NULL);
    ~CCheats(void);

#ifdef tofix
    bool IsCheatMessage ( MSG * msg );
#endif
    void ApplyCheats(CMipsMemory * MMU);
    void ApplyGSButton(CMipsMemory * MMU);
    void LoadCheats(bool DisableSelected, CPlugins * Plugins);
#ifdef tofix
    void SelectCheats(HWND hParent, bool BlockExecution);
    inline bool CheatsSlectionChanged(void) const { return m_CheatSelectionChanged; }
#endif

private:
    struct GAMESHARK_CODE
    {
        uint32_t Command;
        uint16_t Value;
    };

    typedef std::vector<GAMESHARK_CODE> CODES;
    typedef std::vector<CODES>          CODES_ARRAY;

    enum { MaxCheats = 50000 };
    void LoadPermCheats(CPlugins * Plugins);

#ifdef tofix
    static int CALLBACK CheatAddProc(HWND hDlg, uint32_t uMsg, uint32_t wParam, uint32_t lParam);
    static int CALLBACK CheatListProc(HWND hDlg, uint32_t uMsg, uint32_t wParam, uint32_t lParam);
    static int CALLBACK ManageCheatsProc(HWND hDlg, uint32_t uMsg, uint32_t wParam, uint32_t lParam);
    static int CALLBACK CheatsCodeExProc(HWND hDlg, uint32_t uMsg, uint32_t wParam, uint32_t lParam);
    static int CALLBACK CheatsCodeQuantProc(HWND hDlg, uint32_t uMsg, uint32_t wParam, uint32_t lParam);

    //information about the gui for selecting cheats
    HWND    m_Window, m_hSelectCheat, m_AddCheat, m_hCheatTree, m_hSelectedItem;
#endif
    const CN64Rom * m_Rom;
#ifdef tofix
    void          * const m_rcList, *const m_rcAdd;
    int           m_MinSizeDlg, m_MaxSizeDlg;
    int           m_EditCheat;
    bool          m_DeleteingEntries;
#endif
    CODES_ARRAY   m_Codes;
    bool          m_CheatSelectionChanged;

#ifdef tofix
    //Information about the current cheat we are editing
    stdstr        m_EditName;
    stdstr        m_EditCode;
    stdstr        m_EditOptions;
    stdstr        m_EditNotes;

    enum Dialog_State   { CONTRACTED, EXPANDED } m_DialogState;
    enum TV_CHECK_STATE { TV_STATE_UNKNOWN, TV_STATE_CLEAR, TV_STATE_CHECKED, TV_STATE_INDETERMINATE };
    enum { IDC_MYTREE = 0x500 };
#endif
    enum { MaxGSEntries = 100};

    bool LoadCode(int CheatNo, const char * CheatString);
#ifdef tofix
    void AddCodeLayers(int CheatNumber, const stdstr &CheatName, HWND hParent, bool CheatActive);
    //Reload the cheats from the ini file to the select gui
    void RefreshCheatManager();
    void ChangeChildrenStatus(HWND hParent, bool Checked);
    void CheckParentStatus(HWND hParent);
    static stdstr ReadCodeString(HWND hDlg, bool &validcodes, bool &validoption, bool &nooptions, int &codeformat);
    static stdstr ReadOptionsString(HWND hDlg, bool &validcodes, bool &validoptions, bool &nooptions, int &codeformat);
#endif
    int ApplyCheatEntry(CMipsMemory * MMU, const CODES & CodeEntry, int CurrentEntry, bool Execute);
#ifdef tofix
    void RecordCheatValues(HWND hDlg);
    bool CheatChanged(HWND hDlg);
#endif
    bool IsValid16BitCode(const char * CheatString) const;
    void DeleteCheat(int Index);

    //Get Information about the Cheat
    stdstr GetCheatName(int CheatNo, bool AddExtension) const;
    static bool CheatUsesCodeExtensions(const stdstr &LineEntry);
#ifdef tofix

    //Working with treeview
    static bool  TV_SetCheckState(HWND hwndTreeView, HWND hItem, TV_CHECK_STATE state);
    static int   TV_GetCheckState(HWND hwndTreeView, HWND hItem);
#endif
    static uint32_t AsciiToHex(const char * HexValue);
#ifdef tofix
    static void  MenuSetText(HMENU hMenu, int MenuPos, const wchar_t * Title, const wchar_t * ShortCut);

    //UI Functions
    static stdstr GetDlgItemStr(HWND hDlg, int nIDDlgItem);
#endif
};
