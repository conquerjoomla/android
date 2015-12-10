#pragma once

enum TraceSeverity
{
    TraceAlert		= 0x00000001,
    TraceCritical	= 0x00000002,
    TraceError		= 0x00000003,
    TraceWarning	= 0x00000004,
    TraceNotice		= 0x00000005,
    TraceInfo		= 0x00000006,
    TraceDebug		= 0x00000007,
    TraceVerbose	= 0x00000008,
};

void CloseTrace     ( void ); //Free's all memory associated with trace
void WriteTraceDetails(uint32_t module, uint8_t severity, const char *file, int line, const char *function, const char *format, ...);

#define WriteTrace(m, s, format, ...) \
	if(g_ModuleLogLevel[(m)] >= (s)) \
	{ \
		WriteTraceDetails((m), (s), __FILE__, __LINE__, __FUNCTION__, (format), ## __VA_ARGS__); \
	}

extern uint8_t g_ModuleLogLevel[];
