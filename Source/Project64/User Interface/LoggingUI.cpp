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
#include "stdafx.h"
#include <Project64-core\Logging.h>
#include <prsht.h>

void LoadLogSetting (HKEY hKey,char * String, BOOL * Value);
void SaveLogOptions (void);

LRESULT CALLBACK LogGeneralProc ( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK LogPifProc     ( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK LogRegProc     ( HWND, UINT, WPARAM, LPARAM );

static LOG_OPTIONS g_TempOptions;

void EnterLogOptions(HWND hwndOwner)
{
    PROPSHEETPAGE psp[3];
    PROPSHEETHEADER psh;

    psp[0].dwSize = sizeof(PROPSHEETPAGE);
    psp[0].dwFlags = PSP_USETITLE;
    psp[0].hInstance = GetModuleHandle(NULL);
    psp[0].pszTemplate = MAKEINTRESOURCE(IDD_Logging_Registers);
    psp[0].pfnDlgProc = (DLGPROC)LogRegProc;
    psp[0].pszTitle = "Registers";
    psp[0].lParam = 0;
    psp[0].pfnCallback = NULL;

    psp[1].dwSize = sizeof(PROPSHEETPAGE);
    psp[1].dwFlags = PSP_USETITLE;
    psp[1].hInstance = GetModuleHandle(NULL);
    psp[1].pszTemplate = MAKEINTRESOURCE(IDD_Logging_PifRam);
    psp[1].pfnDlgProc = (DLGPROC)LogPifProc;
    psp[1].pszTitle = "Pif Ram";
    psp[1].lParam = 0;
    psp[1].pfnCallback = NULL;

    psp[2].dwSize = sizeof(PROPSHEETPAGE);
    psp[2].dwFlags = PSP_USETITLE;
    psp[2].hInstance = GetModuleHandle(NULL);
    psp[2].pszTemplate = MAKEINTRESOURCE(IDD_Logging_General);
    psp[2].pfnDlgProc = (DLGPROC)LogGeneralProc;
    psp[2].pszTitle = "General";
    psp[2].lParam = 0;
    psp[2].pfnCallback = NULL;

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
    psh.hwndParent = hwndOwner;
    psh.hInstance = GetModuleHandle(NULL);
    psh.pszCaption = (LPSTR) "Log Options";
    psh.nPages = sizeof(psp) / sizeof(PROPSHEETPAGE);
    psh.nStartPage = 0;
    psh.ppsp = (LPCPROPSHEETPAGE) &psp;
    psh.pfnCallback = NULL;

    LoadLogOptions(&g_TempOptions,TRUE);
    PropertySheet(&psh);
    SaveLogOptions();
    LoadLogOptions(&g_TempOptions, FALSE);
    return;
}

LRESULT CALLBACK LogGeneralProc (HWND hDlg, UINT uMsg, WPARAM /*wParam*/, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        if (g_TempOptions.LogCP0changes) { CheckDlgButton(hDlg,IDC_CP0_WRITE,BST_CHECKED); }
        if (g_TempOptions.LogCP0reads)   { CheckDlgButton(hDlg,IDC_CP0_READ,BST_CHECKED); }
        if (g_TempOptions.LogCache)      { CheckDlgButton(hDlg,IDC_CACHE,BST_CHECKED); }
        if (g_TempOptions.LogExceptions) { CheckDlgButton(hDlg,IDC_EXCEPTIONS,BST_CHECKED); }
        if (g_TempOptions.NoInterrupts)  { CheckDlgButton(hDlg,IDC_INTERRUPTS,BST_CHECKED); }
        if (g_TempOptions.LogTLB)        { CheckDlgButton(hDlg,IDC_TLB,BST_CHECKED); }
        if (g_TempOptions.LogRomHeader)  { CheckDlgButton(hDlg,IDC_ROM_HEADER,BST_CHECKED); }
        if (g_TempOptions.LogUnknown)    { CheckDlgButton(hDlg,IDC_UNKOWN,BST_CHECKED); }
        break;
    case WM_NOTIFY:
        if (((NMHDR FAR *) lParam)->code != PSN_APPLY) { break; }
        g_TempOptions.LogCP0changes = IsDlgButtonChecked(hDlg,IDC_CP0_WRITE) == BST_CHECKED?TRUE:FALSE;
        g_TempOptions.LogCP0reads   = IsDlgButtonChecked(hDlg,IDC_CP0_READ) == BST_CHECKED?TRUE:FALSE;
        g_TempOptions.LogCache      = IsDlgButtonChecked(hDlg,IDC_CACHE) == BST_CHECKED?TRUE:FALSE;
        g_TempOptions.LogExceptions = IsDlgButtonChecked(hDlg,IDC_EXCEPTIONS) == BST_CHECKED?TRUE:FALSE;
        g_TempOptions.NoInterrupts  = IsDlgButtonChecked(hDlg,IDC_INTERRUPTS) == BST_CHECKED?TRUE:FALSE;
        g_TempOptions.LogTLB        = IsDlgButtonChecked(hDlg,IDC_TLB) == BST_CHECKED?TRUE:FALSE;
        g_TempOptions.LogRomHeader  = IsDlgButtonChecked(hDlg,IDC_ROM_HEADER) == BST_CHECKED?TRUE:FALSE;
        g_TempOptions.LogUnknown    = IsDlgButtonChecked(hDlg,IDC_UNKOWN) == BST_CHECKED?TRUE:FALSE;
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

LRESULT CALLBACK LogPifProc (HWND hDlg, UINT uMsg, WPARAM /*wParam*/, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        if (g_TempOptions.LogPRDMAOperations)   { CheckDlgButton(hDlg,IDC_SI_DMA,BST_CHECKED); }
        if (g_TempOptions.LogPRDirectMemLoads)  { CheckDlgButton(hDlg,IDC_DIRECT_WRITE,BST_CHECKED); }
        if (g_TempOptions.LogPRDMAMemLoads)     { CheckDlgButton(hDlg,IDC_DMA_WRITE,BST_CHECKED); }
        if (g_TempOptions.LogPRDirectMemStores) { CheckDlgButton(hDlg,IDC_DIRECT_READ,BST_CHECKED); }
        if (g_TempOptions.LogPRDMAMemStores)    { CheckDlgButton(hDlg,IDC_DMA_READ,BST_CHECKED); }
        if (g_TempOptions.LogControllerPak)     { CheckDlgButton(hDlg,IDC_CONT_PAK,BST_CHECKED); }
        break;
    case WM_NOTIFY:
        if (((NMHDR FAR *) lParam)->code != PSN_APPLY)
        {
            break;
        }
        g_TempOptions.LogPRDMAOperations   = IsDlgButtonChecked(hDlg,IDC_SI_DMA) == BST_CHECKED?TRUE:FALSE;
        g_TempOptions.LogPRDirectMemLoads  = IsDlgButtonChecked(hDlg,IDC_DIRECT_WRITE) == BST_CHECKED?TRUE:FALSE;
        g_TempOptions.LogPRDMAMemLoads     = IsDlgButtonChecked(hDlg,IDC_DMA_WRITE) == BST_CHECKED?TRUE:FALSE;
        g_TempOptions.LogPRDirectMemStores = IsDlgButtonChecked(hDlg,IDC_DIRECT_READ) == BST_CHECKED?TRUE:FALSE;
        g_TempOptions.LogPRDMAMemStores    = IsDlgButtonChecked(hDlg,IDC_DMA_READ) == BST_CHECKED?TRUE:FALSE;
        g_TempOptions.LogControllerPak    = IsDlgButtonChecked(hDlg,IDC_CONT_PAK) == BST_CHECKED?TRUE:FALSE;
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

LRESULT CALLBACK LogRegProc (HWND hDlg, UINT uMsg, WPARAM /*wParam*/, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        if (g_TempOptions.LogRDRamRegisters)  { CheckDlgButton(hDlg,IDC_RDRAM,BST_CHECKED); }
        if (g_TempOptions.LogSPRegisters)     { CheckDlgButton(hDlg,IDC_SP_REG,BST_CHECKED); }
        if (g_TempOptions.LogDPCRegisters)    { CheckDlgButton(hDlg,IDC_DPC_REG,BST_CHECKED); }
        if (g_TempOptions.LogDPSRegisters)    { CheckDlgButton(hDlg,IDC_DPS_REG,BST_CHECKED); }
        if (g_TempOptions.LogMIPSInterface)   { CheckDlgButton(hDlg,IDC_MI_REG,BST_CHECKED); }
        if (g_TempOptions.LogVideoInterface)  { CheckDlgButton(hDlg,IDC_VI_REG,BST_CHECKED); }
        if (g_TempOptions.LogAudioInterface)  { CheckDlgButton(hDlg,IDC_AI_REG,BST_CHECKED); }
        if (g_TempOptions.LogPerInterface)    { CheckDlgButton(hDlg,IDC_PI_REG,BST_CHECKED); }
        if (g_TempOptions.LogRDRAMInterface)  { CheckDlgButton(hDlg,IDC_RI_REG,BST_CHECKED); }
        if (g_TempOptions.LogSerialInterface) { CheckDlgButton(hDlg,IDC_SI_REG,BST_CHECKED); }
        break;
    case WM_NOTIFY:
        if (((NMHDR FAR *) lParam)->code != PSN_APPLY)
        {
            break;
        }
        g_TempOptions.LogRDRamRegisters  = IsDlgButtonChecked(hDlg,IDC_RDRAM) == BST_CHECKED?TRUE:FALSE;
        g_TempOptions.LogSPRegisters     = IsDlgButtonChecked(hDlg,IDC_SP_REG) == BST_CHECKED?TRUE:FALSE;
        g_TempOptions.LogDPCRegisters    = IsDlgButtonChecked(hDlg,IDC_DPC_REG) == BST_CHECKED?TRUE:FALSE;
        g_TempOptions.LogDPSRegisters    = IsDlgButtonChecked(hDlg,IDC_DPS_REG) == BST_CHECKED?TRUE:FALSE;
        g_TempOptions.LogMIPSInterface   = IsDlgButtonChecked(hDlg,IDC_MI_REG) == BST_CHECKED?TRUE:FALSE;
        g_TempOptions.LogVideoInterface  = IsDlgButtonChecked(hDlg,IDC_VI_REG) == BST_CHECKED?TRUE:FALSE;
        g_TempOptions.LogAudioInterface  = IsDlgButtonChecked(hDlg,IDC_AI_REG) == BST_CHECKED?TRUE:FALSE;
        g_TempOptions.LogPerInterface    = IsDlgButtonChecked(hDlg,IDC_PI_REG) == BST_CHECKED?TRUE:FALSE;
        g_TempOptions.LogRDRAMInterface  = IsDlgButtonChecked(hDlg,IDC_RI_REG) == BST_CHECKED?TRUE:FALSE;
        g_TempOptions.LogSerialInterface = IsDlgButtonChecked(hDlg,IDC_SI_REG) == BST_CHECKED?TRUE:FALSE;
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

void SaveLogSetting (HKEY hKey,char * String, BOOL Value)
{
    DWORD StoreValue = Value;
    RegSetValueEx(hKey,String,0,REG_DWORD,(CONST BYTE *)&StoreValue,sizeof(DWORD));
}

void SaveLogOptions (void)
{
    long lResult;
    HKEY hKeyResults = 0;
    DWORD Disposition = 0;
    char String[200];

    sprintf(String,"Software\\N64 Emulation\\%s\\Logging",g_Settings->LoadStringVal(Setting_ApplicationName).c_str());
    lResult = RegCreateKeyEx( HKEY_CURRENT_USER,String,0,"", REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS,NULL,&hKeyResults,&Disposition);

    SaveLogSetting(hKeyResults,"Log RDRAM",g_TempOptions.LogRDRamRegisters);
    SaveLogSetting(hKeyResults,"Log SP",g_TempOptions.LogSPRegisters);
    SaveLogSetting(hKeyResults,"Log DP Command",g_TempOptions.LogDPCRegisters);
    SaveLogSetting(hKeyResults,"Log DP Span",g_TempOptions.LogDPSRegisters);
    SaveLogSetting(hKeyResults,"Log MIPS Interface (MI)",g_TempOptions.LogMIPSInterface);
    SaveLogSetting(hKeyResults,"Log Video Interface (VI)",g_TempOptions.LogVideoInterface);
    SaveLogSetting(hKeyResults,"Log Audio Interface (AI)",g_TempOptions.LogAudioInterface);
    SaveLogSetting(hKeyResults,"Log Peripheral Interface (PI)",g_TempOptions.LogPerInterface);
    SaveLogSetting(hKeyResults,"Log RDRAM Interface (RI)",g_TempOptions.LogRDRAMInterface);
    SaveLogSetting(hKeyResults,"Log Serial Interface (SI)",g_TempOptions.LogSerialInterface);
    SaveLogSetting(hKeyResults,"Log PifRam DMA Operations",g_TempOptions.LogPRDMAOperations);
    SaveLogSetting(hKeyResults,"Log PifRam Direct Memory Loads",g_TempOptions.LogPRDirectMemLoads);
    SaveLogSetting(hKeyResults,"Log PifRam DMA Memory Loads",g_TempOptions.LogPRDMAMemLoads);
    SaveLogSetting(hKeyResults,"Log PifRam Direct Memory Stores",g_TempOptions.LogPRDirectMemStores);
    SaveLogSetting(hKeyResults,"Log PifRam DMA Memory Stores",g_TempOptions.LogPRDMAMemStores);
    SaveLogSetting(hKeyResults,"Log Controller Pak",g_TempOptions.LogControllerPak);
    SaveLogSetting(hKeyResults,"Log CP0 changes",g_TempOptions.LogCP0changes);
    SaveLogSetting(hKeyResults,"Log CP0 reads",g_TempOptions.LogCP0reads);
    SaveLogSetting(hKeyResults,"Log Exceptions",g_TempOptions.LogExceptions);
    SaveLogSetting(hKeyResults,"No Interrupts",g_TempOptions.NoInterrupts);
    SaveLogSetting(hKeyResults,"Log TLB",g_TempOptions.LogTLB);
    SaveLogSetting(hKeyResults,"Log Cache Operations",g_TempOptions.LogCache);
    SaveLogSetting(hKeyResults,"Log Rom Header",g_TempOptions.LogRomHeader);
    SaveLogSetting(hKeyResults,"Log Unknown access",g_TempOptions.LogUnknown);

    RegCloseKey(hKeyResults);
}