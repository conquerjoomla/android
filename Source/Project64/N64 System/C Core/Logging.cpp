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

#include <prsht.h>

void LoadLogSetting (HKEY hKey,char * String, BOOL * Value);
void SaveLogOptions (void);

LRESULT CALLBACK LogGeneralProc ( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK LogPifProc     ( HWND, UINT, WPARAM, LPARAM );
LRESULT CALLBACK LogRegProc     ( HWND, UINT, WPARAM, LPARAM );

LOG_OPTIONS TempOptions;

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

    LoadLogOptions(&TempOptions,TRUE);
#if defined(WINDOWS_UI)
    PropertySheet(&psh);
#else
    g_Notify -> BreakPoint(__FILEW__, __LINE__);
#endif
    SaveLogOptions();
	LoadLogOptions(&LogOptions, FALSE);
	return;
}

LRESULT CALLBACK LogGeneralProc (HWND hDlg, UINT uMsg, WPARAM /*wParam*/, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		if (TempOptions.LogCP0changes) { CheckDlgButton(hDlg,IDC_CP0_WRITE,BST_CHECKED); }
		if (TempOptions.LogCP0reads)   { CheckDlgButton(hDlg,IDC_CP0_READ,BST_CHECKED); }
		if (TempOptions.LogCache)      { CheckDlgButton(hDlg,IDC_CACHE,BST_CHECKED); }
		if (TempOptions.LogExceptions) { CheckDlgButton(hDlg,IDC_EXCEPTIONS,BST_CHECKED); }
		if (TempOptions.NoInterrupts)  { CheckDlgButton(hDlg,IDC_INTERRUPTS,BST_CHECKED); }
		if (TempOptions.LogTLB)        { CheckDlgButton(hDlg,IDC_TLB,BST_CHECKED); }
		if (TempOptions.LogRomHeader)  { CheckDlgButton(hDlg,IDC_ROM_HEADER,BST_CHECKED); }
		if (TempOptions.LogUnknown)    { CheckDlgButton(hDlg,IDC_UNKOWN,BST_CHECKED); }
		break;
	case WM_NOTIFY:
		if (((NMHDR FAR *) lParam)->code != PSN_APPLY) { break; }
		TempOptions.LogCP0changes = IsDlgButtonChecked(hDlg,IDC_CP0_WRITE) == BST_CHECKED?TRUE:FALSE;
		TempOptions.LogCP0reads   = IsDlgButtonChecked(hDlg,IDC_CP0_READ) == BST_CHECKED?TRUE:FALSE;
		TempOptions.LogCache      = IsDlgButtonChecked(hDlg,IDC_CACHE) == BST_CHECKED?TRUE:FALSE;
		TempOptions.LogExceptions = IsDlgButtonChecked(hDlg,IDC_EXCEPTIONS) == BST_CHECKED?TRUE:FALSE;
		TempOptions.NoInterrupts  = IsDlgButtonChecked(hDlg,IDC_INTERRUPTS) == BST_CHECKED?TRUE:FALSE;
		TempOptions.LogTLB        = IsDlgButtonChecked(hDlg,IDC_TLB) == BST_CHECKED?TRUE:FALSE;
		TempOptions.LogRomHeader  = IsDlgButtonChecked(hDlg,IDC_ROM_HEADER) == BST_CHECKED?TRUE:FALSE;
		TempOptions.LogUnknown    = IsDlgButtonChecked(hDlg,IDC_UNKOWN) == BST_CHECKED?TRUE:FALSE;
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
		if (TempOptions.LogPRDMAOperations)   { CheckDlgButton(hDlg,IDC_SI_DMA,BST_CHECKED); }
		if (TempOptions.LogPRDirectMemLoads)  { CheckDlgButton(hDlg,IDC_DIRECT_WRITE,BST_CHECKED); }
		if (TempOptions.LogPRDMAMemLoads)     { CheckDlgButton(hDlg,IDC_DMA_WRITE,BST_CHECKED); }
		if (TempOptions.LogPRDirectMemStores) { CheckDlgButton(hDlg,IDC_DIRECT_READ,BST_CHECKED); }
		if (TempOptions.LogPRDMAMemStores)    { CheckDlgButton(hDlg,IDC_DMA_READ,BST_CHECKED); }
		if (TempOptions.LogControllerPak)     { CheckDlgButton(hDlg,IDC_CONT_PAK,BST_CHECKED); }
		break;
	case WM_NOTIFY:
		if (((NMHDR FAR *) lParam)->code != PSN_APPLY)
		{
			break;
		}		
		TempOptions.LogPRDMAOperations   = IsDlgButtonChecked(hDlg,IDC_SI_DMA) == BST_CHECKED?TRUE:FALSE;
		TempOptions.LogPRDirectMemLoads  = IsDlgButtonChecked(hDlg,IDC_DIRECT_WRITE) == BST_CHECKED?TRUE:FALSE;
		TempOptions.LogPRDMAMemLoads     = IsDlgButtonChecked(hDlg,IDC_DMA_WRITE) == BST_CHECKED?TRUE:FALSE;
		TempOptions.LogPRDirectMemStores = IsDlgButtonChecked(hDlg,IDC_DIRECT_READ) == BST_CHECKED?TRUE:FALSE;
		TempOptions.LogPRDMAMemStores    = IsDlgButtonChecked(hDlg,IDC_DMA_READ) == BST_CHECKED?TRUE:FALSE;
		TempOptions.LogControllerPak    = IsDlgButtonChecked(hDlg,IDC_CONT_PAK) == BST_CHECKED?TRUE:FALSE;
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
		if (TempOptions.LogRDRamRegisters)  { CheckDlgButton(hDlg,IDC_RDRAM,BST_CHECKED); }
		if (TempOptions.LogSPRegisters)     { CheckDlgButton(hDlg,IDC_SP_REG,BST_CHECKED); }
		if (TempOptions.LogDPCRegisters)    { CheckDlgButton(hDlg,IDC_DPC_REG,BST_CHECKED); }
		if (TempOptions.LogDPSRegisters)    { CheckDlgButton(hDlg,IDC_DPS_REG,BST_CHECKED); }
		if (TempOptions.LogMIPSInterface)   { CheckDlgButton(hDlg,IDC_MI_REG,BST_CHECKED); }
		if (TempOptions.LogVideoInterface)  { CheckDlgButton(hDlg,IDC_VI_REG,BST_CHECKED); }
		if (TempOptions.LogAudioInterface)  { CheckDlgButton(hDlg,IDC_AI_REG,BST_CHECKED); }
		if (TempOptions.LogPerInterface)    { CheckDlgButton(hDlg,IDC_PI_REG,BST_CHECKED); }
		if (TempOptions.LogRDRAMInterface)  { CheckDlgButton(hDlg,IDC_RI_REG,BST_CHECKED); }
		if (TempOptions.LogSerialInterface) { CheckDlgButton(hDlg,IDC_SI_REG,BST_CHECKED); }
		break;
	case WM_NOTIFY:
		if (((NMHDR FAR *) lParam)->code != PSN_APPLY)
		{
			break;
		}
		TempOptions.LogRDRamRegisters  = IsDlgButtonChecked(hDlg,IDC_RDRAM) == BST_CHECKED?TRUE:FALSE;
		TempOptions.LogSPRegisters     = IsDlgButtonChecked(hDlg,IDC_SP_REG) == BST_CHECKED?TRUE:FALSE;
		TempOptions.LogDPCRegisters    = IsDlgButtonChecked(hDlg,IDC_DPC_REG) == BST_CHECKED?TRUE:FALSE;
		TempOptions.LogDPSRegisters    = IsDlgButtonChecked(hDlg,IDC_DPS_REG) == BST_CHECKED?TRUE:FALSE;
		TempOptions.LogMIPSInterface   = IsDlgButtonChecked(hDlg,IDC_MI_REG) == BST_CHECKED?TRUE:FALSE;
		TempOptions.LogVideoInterface  = IsDlgButtonChecked(hDlg,IDC_VI_REG) == BST_CHECKED?TRUE:FALSE;
		TempOptions.LogAudioInterface  = IsDlgButtonChecked(hDlg,IDC_AI_REG) == BST_CHECKED?TRUE:FALSE;
		TempOptions.LogPerInterface    = IsDlgButtonChecked(hDlg,IDC_PI_REG) == BST_CHECKED?TRUE:FALSE;
		TempOptions.LogRDRAMInterface  = IsDlgButtonChecked(hDlg,IDC_RI_REG) == BST_CHECKED?TRUE:FALSE;
		TempOptions.LogSerialInterface = IsDlgButtonChecked(hDlg,IDC_SI_REG) == BST_CHECKED?TRUE:FALSE;
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
	
	SaveLogSetting(hKeyResults,"Log RDRAM",TempOptions.LogRDRamRegisters);
	SaveLogSetting(hKeyResults,"Log SP",TempOptions.LogSPRegisters);
	SaveLogSetting(hKeyResults,"Log DP Command",TempOptions.LogDPCRegisters);
	SaveLogSetting(hKeyResults,"Log DP Span",TempOptions.LogDPSRegisters);
	SaveLogSetting(hKeyResults,"Log MIPS Interface (MI)",TempOptions.LogMIPSInterface);
	SaveLogSetting(hKeyResults,"Log Video Interface (VI)",TempOptions.LogVideoInterface);
	SaveLogSetting(hKeyResults,"Log Audio Interface (AI)",TempOptions.LogAudioInterface);
	SaveLogSetting(hKeyResults,"Log Peripheral Interface (PI)",TempOptions.LogPerInterface);
	SaveLogSetting(hKeyResults,"Log RDRAM Interface (RI)",TempOptions.LogRDRAMInterface);
	SaveLogSetting(hKeyResults,"Log Serial Interface (SI)",TempOptions.LogSerialInterface);
	SaveLogSetting(hKeyResults,"Log PifRam DMA Operations",TempOptions.LogPRDMAOperations);
	SaveLogSetting(hKeyResults,"Log PifRam Direct Memory Loads",TempOptions.LogPRDirectMemLoads);
	SaveLogSetting(hKeyResults,"Log PifRam DMA Memory Loads",TempOptions.LogPRDMAMemLoads);
	SaveLogSetting(hKeyResults,"Log PifRam Direct Memory Stores",TempOptions.LogPRDirectMemStores);
	SaveLogSetting(hKeyResults,"Log PifRam DMA Memory Stores",TempOptions.LogPRDMAMemStores);
	SaveLogSetting(hKeyResults,"Log Controller Pak",TempOptions.LogControllerPak);
	SaveLogSetting(hKeyResults,"Log CP0 changes",TempOptions.LogCP0changes);
	SaveLogSetting(hKeyResults,"Log CP0 reads",TempOptions.LogCP0reads);
	SaveLogSetting(hKeyResults,"Log Exceptions",TempOptions.LogExceptions);
	SaveLogSetting(hKeyResults,"No Interrupts",TempOptions.NoInterrupts);
	SaveLogSetting(hKeyResults,"Log TLB",TempOptions.LogTLB);
	SaveLogSetting(hKeyResults,"Log Cache Operations",TempOptions.LogCache);
	SaveLogSetting(hKeyResults,"Log Rom Header",TempOptions.LogRomHeader);
	SaveLogSetting(hKeyResults,"Log Unknown access",TempOptions.LogUnknown);
	
	RegCloseKey(hKeyResults);
}