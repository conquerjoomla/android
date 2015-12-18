#pragma once

#define _T(x)       __T(x)

#ifdef _WIN32

#define __T(x)      L ## x

#else

#define __T(x)      x


#include <alloca.h>
#include <stdarg.h>

#define _stricmp strcasecmp
#define _strnicmp strncasecmp
#define _snprintf snprintf
#define GetCurrentThreadId pthread_self

int _vscprintf (const char * format, va_list pargs);

#endif