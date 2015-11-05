#include "stdafx.h"
#include "logging.h"
#include <Common/path.h>
#include <Project64-core\N64 System\System Globals.h>
#include <Project64-core\N64 System\Mips\TranslateVaddr.h>
#include <Project64-core\N64 System\Mips\Memory Class.h>
#include <Project64-core\N64 System\N64 Rom Class.h>
#include <Windows.h>

static HANDLE g_hLogFile = NULL;
LOG_OPTIONS g_LogOptions;

void StartLog (void)
{
    if (!g_LogOptions.GenerateLog)
    {
        StopLog();
        return;
    }
    if (g_hLogFile)
    {
        return;
    }

    CPath LogFile(CPath::MODULE_DIRECTORY);
    LogFile.AppendDirectory("Logs");
    LogFile.SetNameExtension("cpudebug.log");

    g_hLogFile = CreateFile(LogFile,GENERIC_WRITE, FILE_SHARE_READ,NULL,CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    SetFilePointer(g_hLogFile,0,NULL,FILE_BEGIN);
}

void StopLog (void)
{
    if (g_hLogFile)
    {
        CloseHandle(g_hLogFile);
    }
    g_hLogFile = NULL;
}

void LoadLogSetting (HKEY hKey,char * String, bool * Value)
{
    DWORD Type, dwResult, Bytes = 4;
    long lResult;

    lResult = RegQueryValueEx(hKey,String,0,&Type,(LPBYTE)(&dwResult),&Bytes);
    if (Type == REG_DWORD && lResult == ERROR_SUCCESS)
    {
        *Value = dwResult != 0;
    }
    else
    {
        *Value = FALSE;
    }
}

void LoadLogOptions (LOG_OPTIONS * g_LogOptions, bool AlwaysFill)
{
    long lResult;
    HKEY hKeyResults = 0;
    char String[200];

    sprintf(String,"Software\\N64 Emulation\\%s\\Logging",g_Settings->LoadStringVal(Setting_ApplicationName).c_str());
    lResult = RegOpenKeyEx( HKEY_CURRENT_USER,String,0,KEY_ALL_ACCESS,
        &hKeyResults);

    if (lResult == ERROR_SUCCESS)
    {
        //LoadLogSetting(hKeyResults,"Generate Log File",&g_LogOptions->GenerateLog);
        if (g_LogOptions->GenerateLog || AlwaysFill)
        {
            LoadLogSetting(hKeyResults,"Log RDRAM",&g_LogOptions->LogRDRamRegisters);
            LoadLogSetting(hKeyResults,"Log SP",&g_LogOptions->LogSPRegisters);
            LoadLogSetting(hKeyResults,"Log DP Command",&g_LogOptions->LogDPCRegisters);
            LoadLogSetting(hKeyResults,"Log DP Span",&g_LogOptions->LogDPSRegisters);
            LoadLogSetting(hKeyResults,"Log MIPS Interface (MI)",&g_LogOptions->LogMIPSInterface);
            LoadLogSetting(hKeyResults,"Log Video Interface (VI)",&g_LogOptions->LogVideoInterface);
            LoadLogSetting(hKeyResults,"Log Audio Interface (AI)",&g_LogOptions->LogAudioInterface);
            LoadLogSetting(hKeyResults,"Log Peripheral Interface (PI)",&g_LogOptions->LogPerInterface);
            LoadLogSetting(hKeyResults,"Log RDRAM Interface (RI)",&g_LogOptions->LogRDRAMInterface);
            LoadLogSetting(hKeyResults,"Log Serial Interface (SI)",&g_LogOptions->LogSerialInterface);
            LoadLogSetting(hKeyResults,"Log PifRam DMA Operations",&g_LogOptions->LogPRDMAOperations);
            LoadLogSetting(hKeyResults,"Log PifRam Direct Memory Loads",&g_LogOptions->LogPRDirectMemLoads);
            LoadLogSetting(hKeyResults,"Log PifRam DMA Memory Loads",&g_LogOptions->LogPRDMAMemLoads);
            LoadLogSetting(hKeyResults,"Log PifRam Direct Memory Stores",&g_LogOptions->LogPRDirectMemStores);
            LoadLogSetting(hKeyResults,"Log PifRam DMA Memory Stores",&g_LogOptions->LogPRDMAMemStores);
            LoadLogSetting(hKeyResults,"Log Controller Pak",&g_LogOptions->LogControllerPak);
            LoadLogSetting(hKeyResults,"Log CP0 changes",&g_LogOptions->LogCP0changes);
            LoadLogSetting(hKeyResults,"Log CP0 reads",&g_LogOptions->LogCP0reads);
            LoadLogSetting(hKeyResults,"Log Exceptions",&g_LogOptions->LogExceptions);
            LoadLogSetting(hKeyResults,"No Interrupts",&g_LogOptions->NoInterrupts);
            LoadLogSetting(hKeyResults,"Log TLB",&g_LogOptions->LogTLB);
            LoadLogSetting(hKeyResults,"Log Cache Operations",&g_LogOptions->LogCache);
            LoadLogSetting(hKeyResults,"Log Rom Header",&g_LogOptions->LogRomHeader);
            LoadLogSetting(hKeyResults,"Log Unknown access",&g_LogOptions->LogUnknown);
            return;
        }
    }

    g_LogOptions->GenerateLog          = FALSE;
    g_LogOptions->LogRDRamRegisters    = FALSE;
    g_LogOptions->LogSPRegisters       = FALSE;
    g_LogOptions->LogDPCRegisters      = FALSE;
    g_LogOptions->LogDPSRegisters      = FALSE;
    g_LogOptions->LogMIPSInterface     = FALSE;
    g_LogOptions->LogVideoInterface    = FALSE;
    g_LogOptions->LogAudioInterface    = FALSE;
    g_LogOptions->LogPerInterface      = FALSE;
    g_LogOptions->LogRDRAMInterface    = FALSE;
    g_LogOptions->LogSerialInterface   = FALSE;

    g_LogOptions->LogPRDMAOperations   = FALSE;
    g_LogOptions->LogPRDirectMemLoads  = FALSE;
    g_LogOptions->LogPRDMAMemLoads     = FALSE;
    g_LogOptions->LogPRDirectMemStores = FALSE;
    g_LogOptions->LogPRDMAMemStores    = FALSE;
    g_LogOptions->LogControllerPak     = FALSE;

    g_LogOptions->LogCP0changes        = FALSE;
    g_LogOptions->LogCP0reads          = FALSE;
    g_LogOptions->LogCache             = FALSE;
    g_LogOptions->LogExceptions        = FALSE;
    g_LogOptions->NoInterrupts         = FALSE;
    g_LogOptions->LogTLB               = FALSE;
    g_LogOptions->LogRomHeader         = FALSE;
    g_LogOptions->LogUnknown           = FALSE;
}

void Log_LW (uint32_t PC, uint32_t VAddr)
{
    if (!g_LogOptions.GenerateLog)
    {
        return;
    }

    if ( VAddr < 0xA0000000 || VAddr >= 0xC0000000 )
    {
        uint32_t PAddr;
        if (!g_TransVaddr->TranslateVaddr(VAddr,PAddr))
        {
            if (g_LogOptions.LogUnknown)
            {
                LogMessage("%08X: read from unknown ??? (%08X)",PC,VAddr);
            }
            return;
        }
        VAddr = PAddr + 0xA0000000;
    }

    uint32_t Value;
    if ( VAddr >= 0xA0000000 && VAddr < (0xA0000000 + g_MMU->RdramSize()))
    {
        return;
    }
    if ( VAddr >= 0xA3F00000 && VAddr <= 0xA3F00024)
    {
        if (!g_LogOptions.LogRDRamRegisters)
        {
            return;
        }
        g_MMU->LW_VAddr(VAddr,Value);

        switch (VAddr)
        {
        case 0xA3F00000: LogMessage("%08X: read from RDRAM_CONFIG_REG/RDRAM_DEVICE_TYPE_REG (%08X)",PC, Value); return;
        case 0xA3F00004: LogMessage("%08X: read from RDRAM_DEVICE_ID_REG (%08X)",PC, Value); return;
        case 0xA3F00008: LogMessage("%08X: read from RDRAM_DELAY_REG (%08X)",PC, Value); return;
        case 0xA3F0000C: LogMessage("%08X: read from RDRAM_MODE_REG (%08X)",PC, Value); return;
        case 0xA3F00010: LogMessage("%08X: read from RDRAM_REF_INTERVAL_REG (%08X)",PC, Value); return;
        case 0xA3F00014: LogMessage("%08X: read from RDRAM_REF_ROW_REG (%08X)",PC, Value); return;
        case 0xA3F00018: LogMessage("%08X: read from RDRAM_RAS_INTERVAL_REG (%08X)",PC, Value); return;
        case 0xA3F0001C: LogMessage("%08X: read from RDRAM_MIN_INTERVAL_REG (%08X)",PC, Value); return;
        case 0xA3F00020: LogMessage("%08X: read from RDRAM_ADDR_SELECT_REG (%08X)",PC, Value); return;
        case 0xA3F00024: LogMessage("%08X: read from RDRAM_DEVICE_MANUF_REG (%08X)",PC, Value); return;
        }
    }

    if ( VAddr >= 0xA4000000 && VAddr <= 0xA4001FFC )
    {
        return;
    }
    if ( VAddr >= 0xA4040000 && VAddr <= 0xA404001C )
    {
        if (!g_LogOptions.LogSPRegisters)
        {
            return;
        }
        g_MMU->LW_VAddr(VAddr,Value);

        switch (VAddr)
        {
        case 0xA4040000: LogMessage("%08X: read from SP_MEM_ADDR_REG (%08X)",PC, Value); break;
        case 0xA4040004: LogMessage("%08X: read from SP_DRAM_ADDR_REG (%08X)",PC, Value); break;
        case 0xA4040008: LogMessage("%08X: read from SP_RD_LEN_REG (%08X)",PC, Value); break;
        case 0xA404000C: LogMessage("%08X: read from SP_WR_LEN_REG (%08X)",PC, Value); break;
        case 0xA4040010: LogMessage("%08X: read from SP_STATUS_REG (%08X)",PC, Value); break;
        case 0xA4040014: LogMessage("%08X: read from SP_DMA_FULL_REG (%08X)",PC, Value); break;
        case 0xA4040018: LogMessage("%08X: read from SP_DMA_BUSY_REG (%08X)",PC, Value); break;
        case 0xA404001C: LogMessage("%08X: read from SP_SEMAPHORE_REG (%08X)",PC, Value); break;
        }
        return;
    }
    if ( VAddr == 0xA4080000)
    {
        if (!g_LogOptions.LogSPRegisters)
        {
            return;
        }
        g_MMU->LW_VAddr(VAddr,Value);
        LogMessage("%08X: read from SP_PC (%08X)",PC, Value);
        return;
    }
    if (VAddr >= 0xA4100000 && VAddr <= 0xA410001C)
    {
        if (!g_LogOptions.LogDPCRegisters)
        {
            return;
        }
        g_MMU->LW_VAddr(VAddr,Value);

        switch (VAddr)
        {
        case 0xA4100000: LogMessage("%08X: read from DPC_START_REG (%08X)",PC, Value); return;
        case 0xA4100004: LogMessage("%08X: read from DPC_END_REG (%08X)",PC, Value); return;
        case 0xA4100008: LogMessage("%08X: read from DPC_CURRENT_REG (%08X)",PC, Value); return;
        case 0xA410000C: LogMessage("%08X: read from DPC_STATUS_REG (%08X)",PC, Value); return;
        case 0xA4100010: LogMessage("%08X: read from DPC_CLOCK_REG (%08X)",PC, Value); return;
        case 0xA4100014: LogMessage("%08X: read from DPC_BUFBUSY_REG (%08X)",PC, Value); return;
        case 0xA4100018: LogMessage("%08X: read from DPC_PIPEBUSY_REG (%08X)",PC, Value); return;
        case 0xA410001C: LogMessage("%08X: read from DPC_TMEM_REG (%08X)",PC, Value); return;
        }
    }
    if (VAddr >= 0xA4300000 && VAddr <= 0xA430000C)
    {
        if (!g_LogOptions.LogMIPSInterface)
        {
            return;
        }
        g_MMU->LW_VAddr(VAddr,Value);

        switch (VAddr)
        {
        case 0xA4300000: LogMessage("%08X: read from MI_INIT_MODE_REG/MI_MODE_REG (%08X)",PC, Value); return;
        case 0xA4300004: LogMessage("%08X: read from MI_VERSION_REG/MI_NOOP_REG (%08X)",PC, Value); return;
        case 0xA4300008: LogMessage("%08X: read from MI_INTR_REG (%08X)",PC, Value); return;
        case 0xA430000C: LogMessage("%08X: read from MI_INTR_MASK_REG (%08X)",PC, Value); return;
        }
    }
    if (VAddr >= 0xA4400000 && VAddr <= 0xA4400034)
    {
        if (!g_LogOptions.LogVideoInterface)
        {
            return;
        }
        g_MMU->LW_VAddr(VAddr,Value);

        switch (VAddr)
        {
        case 0xA4400000: LogMessage("%08X: read from VI_STATUS_REG/VI_CONTROL_REG (%08X)",PC, Value); return;
        case 0xA4400004: LogMessage("%08X: read from VI_ORIGIN_REG/VI_DRAM_ADDR_REG (%08X)",PC, Value); return;
        case 0xA4400008: LogMessage("%08X: read from VI_WIDTH_REG/VI_H_WIDTH_REG (%08X)",PC, Value); return;
        case 0xA440000C: LogMessage("%08X: read from VI_INTR_REG/VI_V_INTR_REG (%08X)",PC, Value); return;
        case 0xA4400010: LogMessage("%08X: read from VI_CURRENT_REG/VI_V_CURRENT_LINE_REG (%08X)",PC, Value); return;
        case 0xA4400014: LogMessage("%08X: read from VI_BURST_REG/VI_TIMING_REG (%08X)",PC, Value); return;
        case 0xA4400018: LogMessage("%08X: read from VI_V_SYNC_REG (%08X)",PC, Value); return;
        case 0xA440001C: LogMessage("%08X: read from VI_H_SYNC_REG (%08X)",PC, Value); return;
        case 0xA4400020: LogMessage("%08X: read from VI_LEAP_REG/VI_H_SYNC_LEAP_REG (%08X)",PC, Value); return;
        case 0xA4400024: LogMessage("%08X: read from VI_H_START_REG/VI_H_VIDEO_REG (%08X)",PC, Value); return;
        case 0xA4400028: LogMessage("%08X: read from VI_V_START_REG/VI_V_VIDEO_REG (%08X)",PC, Value); return;
        case 0xA440002C: LogMessage("%08X: read from VI_V_BURST_REG (%08X)",PC, Value); return;
        case 0xA4400030: LogMessage("%08X: read from VI_X_SCALE_REG (%08X)",PC, Value); return;
        case 0xA4400034: LogMessage("%08X: read from VI_Y_SCALE_REG (%08X)",PC, Value); return;
        }
    }
    if (VAddr >= 0xA4500000 && VAddr <= 0xA4500014)
    {
        if (!g_LogOptions.LogAudioInterface)
        {
            return;
        }
        g_MMU->LW_VAddr(VAddr,Value);

        switch (VAddr)
        {
        case 0xA4500000: LogMessage("%08X: read from AI_DRAM_ADDR_REG (%08X)",PC, Value); return;
        case 0xA4500004: LogMessage("%08X: read from AI_LEN_REG (%08X)",PC, Value); return;
        case 0xA4500008: LogMessage("%08X: read from AI_CONTROL_REG (%08X)",PC, Value); return;
        case 0xA450000C: LogMessage("%08X: read from AI_STATUS_REG (%08X)",PC, Value); return;
        case 0xA4500010: LogMessage("%08X: read from AI_DACRATE_REG (%08X)",PC, Value); return;
        case 0xA4500014: LogMessage("%08X: read from AI_BITRATE_REG (%08X)",PC, Value); return;
        }
    }
    if (VAddr >= 0xA4600000 && VAddr <= 0xA4600030)
    {
        if (!g_LogOptions.LogPerInterface)
        {
            return;
        }
        g_MMU->LW_VAddr(VAddr,Value);

        switch (VAddr)
        {
        case 0xA4600000: LogMessage("%08X: read from PI_DRAM_ADDR_REG (%08X)",PC, Value); return;
        case 0xA4600004: LogMessage("%08X: read from PI_CART_ADDR_REG (%08X)",PC, Value); return;
        case 0xA4600008: LogMessage("%08X: read from PI_RD_LEN_REG (%08X)",PC, Value); return;
        case 0xA460000C: LogMessage("%08X: read from PI_WR_LEN_REG (%08X)",PC, Value); return;
        case 0xA4600010: LogMessage("%08X: read from PI_STATUS_REG (%08X)",PC, Value); return;
        case 0xA4600014: LogMessage("%08X: read from PI_BSD_DOM1_LAT_REG/PI_DOMAIN1_REG (%08X)",PC, Value); return;
        case 0xA4600018: LogMessage("%08X: read from PI_BSD_DOM1_PWD_REG (%08X)",PC, Value); return;
        case 0xA460001C: LogMessage("%08X: read from PI_BSD_DOM1_PGS_REG (%08X)",PC, Value); return;
        case 0xA4600020: LogMessage("%08X: read from PI_BSD_DOM1_RLS_REG (%08X)",PC, Value); return;
        case 0xA4600024: LogMessage("%08X: read from PI_BSD_DOM2_LAT_REG/PI_DOMAIN2_REG (%08X)",PC, Value); return;
        case 0xA4600028: LogMessage("%08X: read from PI_BSD_DOM2_PWD_REG (%08X)",PC, Value); return;
        case 0xA460002C: LogMessage("%08X: read from PI_BSD_DOM2_PGS_REG (%08X)",PC, Value); return;
        case 0xA4600030: LogMessage("%08X: read from PI_BSD_DOM2_RLS_REG (%08X)",PC, Value); return;
        }
    }
    if (VAddr >= 0xA4700000 && VAddr <= 0xA470001C)
    {
        if (!g_LogOptions.LogRDRAMInterface)
        {
            return;
        }
        g_MMU->LW_VAddr(VAddr,Value);

        switch (VAddr)
        {
        case 0xA4700000: LogMessage("%08X: read from RI_MODE_REG (%08X)",PC, Value); return;
        case 0xA4700004: LogMessage("%08X: read from RI_CONFIG_REG (%08X)",PC, Value); return;
        case 0xA4700008: LogMessage("%08X: read from RI_CURRENT_LOAD_REG (%08X)",PC, Value); return;
        case 0xA470000C: LogMessage("%08X: read from RI_SELECT_REG (%08X)",PC, Value); return;
        case 0xA4700010: LogMessage("%08X: read from RI_REFRESH_REG/RI_COUNT_REG (%08X)",PC, Value); return;
        case 0xA4700014: LogMessage("%08X: read from RI_LATENCY_REG (%08X)",PC, Value); return;
        case 0xA4700018: LogMessage("%08X: read from RI_RERROR_REG (%08X)",PC, Value); return;
        case 0xA470001C: LogMessage("%08X: read from RI_WERROR_REG (%08X)",PC, Value); return;
        }
    }
    if ( VAddr == 0xA4800000)
    {
        if (!g_LogOptions.LogSerialInterface)
        {
            return;
        }
        g_MMU->LW_VAddr(VAddr,Value);
        LogMessage("%08X: read from SI_DRAM_ADDR_REG (%08X)",PC, Value);
        return;
    }
    if ( VAddr == 0xA4800004)
    {
        if (!g_LogOptions.LogSerialInterface)
        {
            return;
        }
        g_MMU->LW_VAddr(VAddr,Value);
        LogMessage("%08X: read from SI_PIF_ADDR_RD64B_REG (%08X)",PC, Value);
        return;
    }
    if ( VAddr == 0xA4800010)
    {
        if (!g_LogOptions.LogSerialInterface)
        {
            return;
        }
        g_MMU->LW_VAddr(VAddr,Value);
        LogMessage("%08X: read from SI_PIF_ADDR_WR64B_REG (%08X)",PC, Value);
        return;
    }
    if ( VAddr == 0xA4800018)
    {
        if (!g_LogOptions.LogSerialInterface)
        {
            return;
        }
        g_MMU->LW_VAddr(VAddr,Value);
        LogMessage("%08X: read from SI_STATUS_REG (%08X)",PC, Value);
        return;
    }
    if ( VAddr >= 0xBFC00000 && VAddr <= 0xBFC007C0 )
    {
        return;
    }
    if ( VAddr >= 0xBFC007C0 && VAddr <= 0xBFC007FC )
    {
        if (!g_LogOptions.LogPRDirectMemLoads)
        {
            return;
        }
        g_MMU->LW_VAddr(VAddr,Value);
        LogMessage("%08X: read word from Pif Ram at 0x%X (%08X)",PC,VAddr - 0xBFC007C0, Value);
        return;
    }
    if ( VAddr >= 0xB0000040 && ((VAddr - 0xB0000000) < g_Rom->GetRomSize()))
    {
        return;
    }
    if ( VAddr >= 0xB0000000 && VAddr < 0xB0000040)
    {
        if (!g_LogOptions.LogRomHeader)
        {
            return;
        }

        g_MMU->LW_VAddr(VAddr,Value);
        switch (VAddr)
        {
        case 0xB0000004: LogMessage("%08X: read from Rom Clock Rate (%08X)",PC, Value); break;
        case 0xB0000008: LogMessage("%08X: read from Rom Boot address offset (%08X)",PC, Value); break;
        case 0xB000000C: LogMessage("%08X: read from Rom Release offset (%08X)",PC, Value); break;
        case 0xB0000010: LogMessage("%08X: read from Rom CRC1 (%08X)",PC, Value); break;
        case 0xB0000014: LogMessage("%08X: read from Rom CRC2 (%08X)",PC, Value); break;
        default: LogMessage("%08X: read from Rom header 0x%X (%08X)",PC, VAddr & 0xFF,Value);  break;
        }
        return;
    }
    if (!g_LogOptions.LogUnknown)
    {
        return;
    }
    LogMessage("%08X: read from unknown ??? (%08X)",PC,VAddr);
}

void Log_SW (uint32_t PC, uint32_t VAddr, uint32_t Value)
{
    if (!g_LogOptions.GenerateLog)
    {
        return;
    }

    if ( VAddr < 0xA0000000 || VAddr >= 0xC0000000 )
    {
        uint32_t PAddr;
        if (!g_TransVaddr->TranslateVaddr(VAddr,PAddr))
        {
            if (g_LogOptions.LogUnknown)
            {
                LogMessage("%08X: Writing 0x%08X to %08X",PC, Value, VAddr );
            }
            return;
        }
        VAddr = PAddr + 0xA0000000;
    }

    if ( VAddr >= 0xA0000000 && VAddr < (0xA0000000 + g_MMU->RdramSize()))
    {
        return;
    }
    if ( VAddr >= 0xA3F00000 && VAddr <= 0xA3F00024)
    {
        if (!g_LogOptions.LogRDRamRegisters)
        {
            return;
        }
        switch (VAddr)
        {
        case 0xA3F00000: LogMessage("%08X: Writing 0x%08X to RDRAM_CONFIG_REG/RDRAM_DEVICE_TYPE_REG",PC, Value ); return;
        case 0xA3F00004: LogMessage("%08X: Writing 0x%08X to RDRAM_DEVICE_ID_REG",PC, Value ); return;
        case 0xA3F00008: LogMessage("%08X: Writing 0x%08X to RDRAM_DELAY_REG",PC, Value ); return;
        case 0xA3F0000C: LogMessage("%08X: Writing 0x%08X to RDRAM_MODE_REG",PC, Value ); return;
        case 0xA3F00010: LogMessage("%08X: Writing 0x%08X to RDRAM_REF_INTERVAL_REG",PC, Value ); return;
        case 0xA3F00014: LogMessage("%08X: Writing 0x%08X to RDRAM_REF_ROW_REG",PC, Value ); return;
        case 0xA3F00018: LogMessage("%08X: Writing 0x%08X to RDRAM_RAS_INTERVAL_REG",PC, Value ); return;
        case 0xA3F0001C: LogMessage("%08X: Writing 0x%08X to RDRAM_MIN_INTERVAL_REG",PC, Value ); return;
        case 0xA3F00020: LogMessage("%08X: Writing 0x%08X to RDRAM_ADDR_SELECT_REG",PC, Value ); return;
        case 0xA3F00024: LogMessage("%08X: Writing 0x%08X to RDRAM_DEVICE_MANUF_REG",PC, Value ); return;
        }
    }
    if ( VAddr >= 0xA4000000 && VAddr <= 0xA4001FFC )
    {
        return;
    }

    if ( VAddr >= 0xA4040000 && VAddr <= 0xA404001C)
    {
        if (!g_LogOptions.LogSPRegisters)
        {
            return;
        }
        switch (VAddr)
        {
        case 0xA4040000: LogMessage("%08X: Writing 0x%08X to SP_MEM_ADDR_REG",PC, Value ); return;
        case 0xA4040004: LogMessage("%08X: Writing 0x%08X to SP_DRAM_ADDR_REG",PC, Value ); return;
        case 0xA4040008: LogMessage("%08X: Writing 0x%08X to SP_RD_LEN_REG",PC, Value ); return;
        case 0xA404000C: LogMessage("%08X: Writing 0x%08X to SP_WR_LEN_REG",PC, Value ); return;
        case 0xA4040010: LogMessage("%08X: Writing 0x%08X to SP_STATUS_REG",PC, Value ); return;
        case 0xA4040014: LogMessage("%08X: Writing 0x%08X to SP_DMA_FULL_REG",PC, Value ); return;
        case 0xA4040018: LogMessage("%08X: Writing 0x%08X to SP_DMA_BUSY_REG",PC, Value ); return;
        case 0xA404001C: LogMessage("%08X: Writing 0x%08X to SP_SEMAPHORE_REG",PC, Value ); return;
        }
    }
    if ( VAddr == 0xA4080000)
    {
        if (!g_LogOptions.LogSPRegisters)
        {
            return;
        }
        LogMessage("%08X: Writing 0x%08X to SP_PC",PC, Value ); return;
    }

    if ( VAddr >= 0xA4100000 && VAddr <= 0xA410001C)
    {
        if (!g_LogOptions.LogDPCRegisters)
        {
            return;
        }
        switch (VAddr)
        {
        case 0xA4100000: LogMessage("%08X: Writing 0x%08X to DPC_START_REG",PC, Value ); return;
        case 0xA4100004: LogMessage("%08X: Writing 0x%08X to DPC_END_REG",PC, Value ); return;
        case 0xA4100008: LogMessage("%08X: Writing 0x%08X to DPC_CURRENT_REG",PC, Value ); return;
        case 0xA410000C: LogMessage("%08X: Writing 0x%08X to DPC_STATUS_REG",PC, Value ); return;
        case 0xA4100010: LogMessage("%08X: Writing 0x%08X to DPC_CLOCK_REG",PC, Value ); return;
        case 0xA4100014: LogMessage("%08X: Writing 0x%08X to DPC_BUFBUSY_REG",PC, Value ); return;
        case 0xA4100018: LogMessage("%08X: Writing 0x%08X to DPC_PIPEBUSY_REG",PC, Value ); return;
        case 0xA410001C: LogMessage("%08X: Writing 0x%08X to DPC_TMEM_REG",PC, Value ); return;
        }
    }

    if ( VAddr >= 0xA4200000 && VAddr <= 0xA420000C)
    {
        if (!g_LogOptions.LogDPSRegisters)
        {
            return;
        }
        switch (VAddr)
        {
        case 0xA4200000: LogMessage("%08X: Writing 0x%08X to DPS_TBIST_REG",PC, Value ); return;
        case 0xA4200004: LogMessage("%08X: Writing 0x%08X to DPS_TEST_MODE_REG",PC, Value ); return;
        case 0xA4200008: LogMessage("%08X: Writing 0x%08X to DPS_BUFTEST_ADDR_REG",PC, Value ); return;
        case 0xA420000C: LogMessage("%08X: Writing 0x%08X to DPS_BUFTEST_DATA_REG",PC, Value ); return;
        }
    }

    if ( VAddr >= 0xA4300000 && VAddr <= 0xA430000C)
    {
        if (!g_LogOptions.LogMIPSInterface)
        {
            return;
        }
        switch (VAddr)
        {
        case 0xA4300000: LogMessage("%08X: Writing 0x%08X to MI_INIT_MODE_REG/MI_MODE_REG",PC, Value ); return;
        case 0xA4300004: LogMessage("%08X: Writing 0x%08X to MI_VERSION_REG/MI_NOOP_REG",PC, Value ); return;
        case 0xA4300008: LogMessage("%08X: Writing 0x%08X to MI_INTR_REG",PC, Value ); return;
        case 0xA430000C: LogMessage("%08X: Writing 0x%08X to MI_INTR_MASK_REG",PC, Value ); return;
        }
    }
    if ( VAddr >= 0xA4400000 && VAddr <= 0xA4400034)
    {
        if (!g_LogOptions.LogVideoInterface)
        {
            return;
        }
        switch (VAddr)
        {
        case 0xA4400000: LogMessage("%08X: Writing 0x%08X to VI_STATUS_REG/VI_CONTROL_REG",PC, Value ); return;
        case 0xA4400004: LogMessage("%08X: Writing 0x%08X to VI_ORIGIN_REG/VI_DRAM_ADDR_REG",PC, Value ); return;
        case 0xA4400008: LogMessage("%08X: Writing 0x%08X to VI_WIDTH_REG/VI_H_WIDTH_REG",PC, Value ); return;
        case 0xA440000C: LogMessage("%08X: Writing 0x%08X to VI_INTR_REG/VI_V_INTR_REG",PC, Value ); return;
        case 0xA4400010: LogMessage("%08X: Writing 0x%08X to VI_CURRENT_REG/VI_V_CURRENT_LINE_REG",PC, Value ); return;
        case 0xA4400014: LogMessage("%08X: Writing 0x%08X to VI_BURST_REG/VI_TIMING_REG",PC, Value ); return;
        case 0xA4400018: LogMessage("%08X: Writing 0x%08X to VI_V_SYNC_REG",PC, Value ); return;
        case 0xA440001C: LogMessage("%08X: Writing 0x%08X to VI_H_SYNC_REG",PC, Value ); return;
        case 0xA4400020: LogMessage("%08X: Writing 0x%08X to VI_LEAP_REG/VI_H_SYNC_LEAP_REG",PC, Value ); return;
        case 0xA4400024: LogMessage("%08X: Writing 0x%08X to VI_H_START_REG/VI_H_VIDEO_REG",PC, Value ); return;
        case 0xA4400028: LogMessage("%08X: Writing 0x%08X to VI_V_START_REG/VI_V_VIDEO_REG",PC, Value ); return;
        case 0xA440002C: LogMessage("%08X: Writing 0x%08X to VI_V_BURST_REG",PC, Value ); return;
        case 0xA4400030: LogMessage("%08X: Writing 0x%08X to VI_X_SCALE_REG",PC, Value ); return;
        case 0xA4400034: LogMessage("%08X: Writing 0x%08X to VI_Y_SCALE_REG",PC, Value ); return;
        }
    }

    if ( VAddr >= 0xA4500000 && VAddr <= 0xA4500014)
    {
        if (!g_LogOptions.LogAudioInterface)
        {
            return;
        }
        switch (VAddr)
        {
        case 0xA4500000: LogMessage("%08X: Writing 0x%08X to AI_DRAM_ADDR_REG",PC, Value ); return;
        case 0xA4500004: LogMessage("%08X: Writing 0x%08X to AI_LEN_REG",PC, Value ); return;
        case 0xA4500008: LogMessage("%08X: Writing 0x%08X to AI_CONTROL_REG",PC, Value ); return;
        case 0xA450000C: LogMessage("%08X: Writing 0x%08X to AI_STATUS_REG",PC, Value ); return;
        case 0xA4500010: LogMessage("%08X: Writing 0x%08X to AI_DACRATE_REG",PC, Value ); return;
        case 0xA4500014: LogMessage("%08X: Writing 0x%08X to AI_BITRATE_REG",PC, Value ); return;
        }
    }

    if ( VAddr >= 0xA4600000 && VAddr <= 0xA4600030)
    {
        if (!g_LogOptions.LogPerInterface)
        {
            return;
        }
        switch (VAddr)
        {
        case 0xA4600000: LogMessage("%08X: Writing 0x%08X to PI_DRAM_ADDR_REG",PC, Value ); return;
        case 0xA4600004: LogMessage("%08X: Writing 0x%08X to PI_CART_ADDR_REG",PC, Value ); return;
        case 0xA4600008: LogMessage("%08X: Writing 0x%08X to PI_RD_LEN_REG",PC, Value ); return;
        case 0xA460000C: LogMessage("%08X: Writing 0x%08X to PI_WR_LEN_REG",PC, Value ); return;
        case 0xA4600010: LogMessage("%08X: Writing 0x%08X to PI_STATUS_REG",PC, Value ); return;
        case 0xA4600014: LogMessage("%08X: Writing 0x%08X to PI_BSD_DOM1_LAT_REG/PI_DOMAIN1_REG",PC, Value ); return;
        case 0xA4600018: LogMessage("%08X: Writing 0x%08X to PI_BSD_DOM1_PWD_REG",PC, Value ); return;
        case 0xA460001C: LogMessage("%08X: Writing 0x%08X to PI_BSD_DOM1_PGS_REG",PC, Value ); return;
        case 0xA4600020: LogMessage("%08X: Writing 0x%08X to PI_BSD_DOM1_RLS_REG",PC, Value ); return;
        case 0xA4600024: LogMessage("%08X: Writing 0x%08X to PI_BSD_DOM2_LAT_REG/PI_DOMAIN2_REG",PC, Value ); return;
        case 0xA4600028: LogMessage("%08X: Writing 0x%08X to PI_BSD_DOM2_PWD_REG",PC, Value ); return;
        case 0xA460002C: LogMessage("%08X: Writing 0x%08X to PI_BSD_DOM2_PGS_REG",PC, Value ); return;
        case 0xA4600030: LogMessage("%08X: Writing 0x%08X to PI_BSD_DOM2_RLS_REG",PC, Value ); return;
        }
    }
    if ( VAddr >= 0xA4700000 && VAddr <= 0xA470001C)
    {
        if (!g_LogOptions.LogRDRAMInterface)
        {
            return;
        }
        switch (VAddr)
        {
        case 0xA4700000: LogMessage("%08X: Writing 0x%08X to RI_MODE_REG",PC, Value ); return;
        case 0xA4700004: LogMessage("%08X: Writing 0x%08X to RI_CONFIG_REG",PC, Value ); return;
        case 0xA4700008: LogMessage("%08X: Writing 0x%08X to RI_CURRENT_LOAD_REG",PC, Value ); return;
        case 0xA470000C: LogMessage("%08X: Writing 0x%08X to RI_SELECT_REG",PC, Value ); return;
        case 0xA4700010: LogMessage("%08X: Writing 0x%08X to RI_REFRESH_REG/RI_COUNT_REG",PC, Value ); return;
        case 0xA4700014: LogMessage("%08X: Writing 0x%08X to RI_LATENCY_REG",PC, Value ); return;
        case 0xA4700018: LogMessage("%08X: Writing 0x%08X to RI_RERROR_REG",PC, Value ); return;
        case 0xA470001C: LogMessage("%08X: Writing 0x%08X to RI_WERROR_REG",PC, Value ); return;
        }
    }
    if ( VAddr == 0xA4800000)
    {
        if (!g_LogOptions.LogSerialInterface)
        {
            return;
        }
        LogMessage("%08X: Writing 0x%08X to SI_DRAM_ADDR_REG",PC, Value ); return;
    }
    if ( VAddr == 0xA4800004)
    {
        if (g_LogOptions.LogPRDMAOperations)
        {
            LogMessage("%08X: A DMA transfer from the PIF ram has occured",PC );
        }
        if (!g_LogOptions.LogSerialInterface)
        {
            return;
        }
        LogMessage("%08X: Writing 0x%08X to SI_PIF_ADDR_RD64B_REG",PC, Value ); return;
    }
    if ( VAddr == 0xA4800010)
    {
        if (g_LogOptions.LogPRDMAOperations)
        {
            LogMessage("%08X: A DMA transfer to the PIF ram has occured",PC );
        }
        if (!g_LogOptions.LogSerialInterface)
        {
            return;
        }
        LogMessage("%08X: Writing 0x%08X to SI_PIF_ADDR_WR64B_REG",PC, Value ); return;
    }
    if ( VAddr == 0xA4800018)
    {
        if (!g_LogOptions.LogSerialInterface)
        {
            return;
        }
        LogMessage("%08X: Writing 0x%08X to SI_STATUS_REG",PC, Value ); return;
    }

    if ( VAddr >= 0xBFC007C0 && VAddr <= 0xBFC007FC )
    {
        if (!g_LogOptions.LogPRDirectMemStores)
        {
            return;
        }
        LogMessage("%08X: Writing 0x%08X to Pif Ram at 0x%X",PC,Value, VAddr - 0xBFC007C0);
        return;
    }
    if (!g_LogOptions.LogUnknown)
    {
        return;
    }
    LogMessage("%08X: Writing 0x%08X to %08X ????",PC, Value, VAddr );
}

void LogMessage (const char * Message, ...)
{
    DWORD dwWritten;
    char Msg[400];
    va_list ap;

    if (!g_Settings->LoadBool(Debugger_Enabled))
    {
        return;
    }
    if (g_hLogFile == NULL)
    {
        return;
    }

    va_start( ap, Message );
    vsprintf( Msg, Message, ap );
    va_end( ap );

    strcat(Msg,"\r\n");

    WriteFile( g_hLogFile,Msg,strlen(Msg),&dwWritten,NULL );
}