#include "stdafx.h"
#ifdef _WIN32
#include <Windows.h>
#endif

SyncEvent::SyncEvent(bool bManualReset)
{
#ifdef _WIN32
    m_Event = CreateEvent(NULL, bManualReset, FALSE, NULL);
#else
	m_Event = new pthread_mutex_t;
	pthread_mutex_init((pthread_mutex_t*)m_Event,NULL);
#endif
}

SyncEvent::~SyncEvent()
{
#ifdef _WIN32
    CloseHandle(m_Event);
#else
	pthread_mutex_destroy((pthread_mutex_t*)m_Event);
	delete (pthread_mutex_t*)m_Event;
#endif
}

void SyncEvent::Trigger()
{
#ifdef _WIN32
	SetEvent(m_Event);
#else
#ifdef tofix
	pthread_cond_signal(&evt.m_condition);
#endif
#endif
}

bool SyncEvent::IsTriggered(int32_t iWaitTime)
{
#ifdef _WIN32
	return (WAIT_OBJECT_0 == WaitForSingleObject(m_Event,iWaitTime));
#else
#ifdef tofix
	//add linux wait
#endif
#endif
}

void SyncEvent::Reset()
{
#ifdef _WIN32
	ResetEvent(m_Event);
#else
#ifdef tofix
	pthread_mutex_unlock(&evt.m_mutex);
#endif
#endif
}

void * SyncEvent::GetHandle()
{
    return m_Event;
}