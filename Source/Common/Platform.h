#pragma once

#ifdef ANDROID
#define _stricmp strcasecmp
#define _strnicmp strncasecmp
#define _snprintf snprintf
#endif