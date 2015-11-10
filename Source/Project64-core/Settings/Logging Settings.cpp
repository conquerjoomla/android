#include "stdafx.h"
#include "Logging Settings.h"

bool CLogSettings::m_GenerateLog = 0;
bool CLogSettings::m_LogRDRamRegisters = 0;
bool CLogSettings::m_LogSPRegisters = 0;
bool CLogSettings::m_LogDPCRegisters = 0;
bool CLogSettings::m_LogDPSRegisters = 0;
bool CLogSettings::m_LogMIPSInterface = 0;
bool CLogSettings::m_LogVideoInterface = 0;
bool CLogSettings::m_LogAudioInterface = 0;
bool CLogSettings::m_LogPerInterface = 0;
bool CLogSettings::m_LogRDRAMInterface = 0;
bool CLogSettings::m_LogSerialInterface = 0;
bool CLogSettings::m_LogPRDMAOperations = 0;
bool CLogSettings::m_LogPRDirectMemLoads = 0;
bool CLogSettings::m_LogPRDMAMemLoads = 0;
bool CLogSettings::m_LogPRDirectMemStores = 0;
bool CLogSettings::m_LogPRDMAMemStores = 0;
bool CLogSettings::m_LogControllerPak = 0;
bool CLogSettings::m_LogCP0changes = 0;
bool CLogSettings::m_LogCP0reads = 0;
bool CLogSettings::m_LogTLB = 0;
bool CLogSettings::m_LogExceptions = 0;
bool CLogSettings::m_NoInterrupts = 0;
bool CLogSettings::m_LogCache = 0;
bool CLogSettings::m_LogRomHeader = 0;
bool CLogSettings::m_LogUnknown = 0;

CLogSettings::CLogSettings()
{
    m_RefCount += 1;
    if (m_RefCount == 1)
    {
        g_Settings->RegisterChangeCB(Logging_GenerateLog,NULL,RefreshSettings);
        g_Settings->RegisterChangeCB(Logging_LogRDRamRegisters,NULL,RefreshSettings);
        g_Settings->RegisterChangeCB(Logging_LogSPRegisters,NULL,RefreshSettings);
        g_Settings->RegisterChangeCB(Logging_LogDPCRegisters,NULL,RefreshSettings);
        g_Settings->RegisterChangeCB(Logging_LogDPSRegisters,NULL,RefreshSettings);
        g_Settings->RegisterChangeCB(Logging_LogMIPSInterface,NULL,RefreshSettings);
        g_Settings->RegisterChangeCB(Logging_LogVideoInterface,NULL,RefreshSettings);
        g_Settings->RegisterChangeCB(Logging_LogAudioInterface,NULL,RefreshSettings);
        g_Settings->RegisterChangeCB(Logging_LogPerInterface,NULL,RefreshSettings);
        g_Settings->RegisterChangeCB(Logging_LogRDRAMInterface,NULL,RefreshSettings);
        g_Settings->RegisterChangeCB(Logging_LogSerialInterface,NULL,RefreshSettings);
        g_Settings->RegisterChangeCB(Logging_LogPRDMAOperations,NULL,RefreshSettings);
        g_Settings->RegisterChangeCB(Logging_LogPRDirectMemLoads,NULL,RefreshSettings);
        g_Settings->RegisterChangeCB(Logging_LogPRDMAMemLoads,NULL,RefreshSettings);
        g_Settings->RegisterChangeCB(Logging_LogPRDirectMemStores,NULL,RefreshSettings);
        g_Settings->RegisterChangeCB(Logging_LogPRDMAMemStores,NULL,RefreshSettings);
        g_Settings->RegisterChangeCB(Logging_LogControllerPak,NULL,RefreshSettings);
        g_Settings->RegisterChangeCB(Logging_LogCP0changes,NULL,RefreshSettings);
        g_Settings->RegisterChangeCB(Logging_LogCP0reads,NULL,RefreshSettings);
        g_Settings->RegisterChangeCB(Logging_LogTLB,NULL,RefreshSettings);
        g_Settings->RegisterChangeCB(Logging_LogExceptions,NULL,RefreshSettings);
        g_Settings->RegisterChangeCB(Logging_NoInterrupts,NULL,RefreshSettings);
        g_Settings->RegisterChangeCB(Logging_LogCache,NULL,RefreshSettings);
        g_Settings->RegisterChangeCB(Logging_LogRomHeader,NULL,RefreshSettings);
        g_Settings->RegisterChangeCB(Logging_LogUnknown,NULL,RefreshSettings);
        RefreshSettings(NULL);
    }
}

CLogSettings::~CLogSettings()
{
    m_RefCount -= 1;
    if (m_RefCount == 0)
    {
        g_Settings->UnregisterChangeCB(Logging_GenerateLog,NULL,RefreshSettings);
        g_Settings->UnregisterChangeCB(Logging_LogRDRamRegisters,NULL,RefreshSettings);
        g_Settings->UnregisterChangeCB(Logging_LogSPRegisters,NULL,RefreshSettings);
        g_Settings->UnregisterChangeCB(Logging_LogDPCRegisters,NULL,RefreshSettings);
        g_Settings->UnregisterChangeCB(Logging_LogDPSRegisters,NULL,RefreshSettings);
        g_Settings->UnregisterChangeCB(Logging_LogMIPSInterface,NULL,RefreshSettings);
        g_Settings->UnregisterChangeCB(Logging_LogVideoInterface,NULL,RefreshSettings);
        g_Settings->UnregisterChangeCB(Logging_LogAudioInterface,NULL,RefreshSettings);
        g_Settings->UnregisterChangeCB(Logging_LogPerInterface,NULL,RefreshSettings);
        g_Settings->UnregisterChangeCB(Logging_LogRDRAMInterface,NULL,RefreshSettings);
        g_Settings->UnregisterChangeCB(Logging_LogSerialInterface,NULL,RefreshSettings);
        g_Settings->UnregisterChangeCB(Logging_LogPRDMAOperations,NULL,RefreshSettings);
        g_Settings->UnregisterChangeCB(Logging_LogPRDirectMemLoads,NULL,RefreshSettings);
        g_Settings->UnregisterChangeCB(Logging_LogPRDMAMemLoads,NULL,RefreshSettings);
        g_Settings->UnregisterChangeCB(Logging_LogPRDirectMemStores,NULL,RefreshSettings);
        g_Settings->UnregisterChangeCB(Logging_LogPRDMAMemStores,NULL,RefreshSettings);
        g_Settings->UnregisterChangeCB(Logging_LogControllerPak,NULL,RefreshSettings);
        g_Settings->UnregisterChangeCB(Logging_LogCP0changes,NULL,RefreshSettings);
        g_Settings->UnregisterChangeCB(Logging_LogCP0reads,NULL,RefreshSettings);
        g_Settings->UnregisterChangeCB(Logging_LogTLB,NULL,RefreshSettings);
        g_Settings->UnregisterChangeCB(Logging_LogExceptions,NULL,RefreshSettings);
        g_Settings->UnregisterChangeCB(Logging_NoInterrupts,NULL,RefreshSettings);
        g_Settings->UnregisterChangeCB(Logging_LogCache,NULL,RefreshSettings);
        g_Settings->UnregisterChangeCB(Logging_LogRomHeader,NULL,RefreshSettings);
        g_Settings->UnregisterChangeCB(Logging_LogUnknown,NULL,RefreshSettings);
    }
}

void CLogSettings::CLogSettings(void *)
{
    m_bBasicMode           = g_Settings->LoadBool(UserInterface_BasicMode);
    m_bDisplayFrameRate    = g_Settings->LoadBool(UserInterface_DisplayFrameRate);

    m_bShowCPUPer          = g_Settings->LoadBool(UserInterface_ShowCPUPer);
    m_bProfiling           = g_Settings->LoadBool(Debugger_ProfileCode);
    m_bShowDListAListCount = g_Settings->LoadBool(Debugger_ShowDListAListCount);
    m_bLimitFPS            = g_Settings->LoadBool(GameRunning_LimitFPS);