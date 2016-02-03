/****************************************************************************
*                                                                           *
* Project64 - A Nintendo 64 emulator.                                      *
* http://www.pj64-emu.com/                                                  *
* Copyright (C) 2012 Project64. All rights reserved.                        *
*                                                                           *
* License:                                                                  *
* GNU/GPLv2 http://www.gnu.org/licenses/gpl-2.0.html                        *
*                                                                           *
****************************************************************************/
#include "stdafx.h"
#include <Project64-core/N64System/N64Class.h>
#include <Project64-core/Notification.h>
#include <Common/Util.h>

void  CN64System::StartEmulationThead()
{
    WriteTrace(TraceN64System, TraceDebug, "Start");
    ThreadInfo * Info = new ThreadInfo;

#ifdef _WIN32
    HANDLE  * hThread = new HANDLE;
    *hThread = NULL;

    //create the needed info into a structure to pass as one parameter
    //for creating a thread
    Info->ThreadHandle = hThread;

    *hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartEmulationThread, Info, 0, (LPDWORD)&Info->ThreadID);
#else
    pthread_t * thread_id = new pthread_t;

    Info->ThreadHandle = (void** )thread_id;
    Info->ThreadID = 0;

    int res = pthread_create(thread_id, NULL, (void *(*)(void *))StartEmulationThread, Info);
    WriteTrace(TraceN64System, TraceDebug, "pthread_create res = %d, ThreadHandle = %X", res, *Info->ThreadHandle);
#endif
    WriteTrace(TraceN64System, TraceDebug, "Done");
}

void CN64System::StartEmulationThread(ThreadInfo * Info)
{
    WriteTrace(TraceN64System, TraceDebug, "Start");
#ifdef _WIN32
    if (g_Settings->LoadBool(Setting_CN64TimeCritical))
    {
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    }

    CoInitialize(NULL);

    EmulationStarting(*Info->ThreadHandle, Info->ThreadID);
    delete ((HANDLE  *)Info->ThreadHandle);

    CoUninitialize();
#else
    EmulationStarting(Info->ThreadHandle, Info->ThreadID);
#endif
    delete Info;
    WriteTrace(TraceN64System, TraceDebug, "Done");
}

void CN64System::CloseCpu()
{
#ifdef _WIN32
    if (m_CPU_Handle == NULL)
    {
        return;
    }

    m_EndEmulation = true;
    if (g_Settings->LoadBool(GameRunning_CPU_Paused))
    {
        m_hPauseEvent.Trigger();
    }

    if (GetCurrentThreadId() == m_CPU_ThreadID)
    {
        ExternalEvent(SysEvent_CloseCPU);
        return;
    }

    HANDLE hThread = m_CPU_Handle;
    m_CPU_Handle = NULL;
    for (int count = 0; count < 200; count++)
    {
        pjutil::Sleep(100);
        if (g_Notify->ProcessGuiMessages())
        {
            return;
        }

        DWORD ExitCode;
        if (GetExitCodeThread(hThread, &ExitCode))
        {
            if (ExitCode != STILL_ACTIVE)
            {
                break;
            }
        }
    }

    if (hThread)
    {
        DWORD ExitCode;
        GetExitCodeThread(hThread, &ExitCode);
        if (ExitCode == STILL_ACTIVE)
        {
            TerminateThread(hThread, 0);
        }
    }
    CloseHandle(hThread);
    CpuStopped();
#endif
}