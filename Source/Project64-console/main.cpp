#include "stdafx.h"
#include "SDL_main.h"
#include <Project64-core/AppInit.h>
#include <Project64-core/Settings/SettingsClass.h>
#include <Project64-core/N64System/N64Class.h>

#if defined(ANDROID)
#include <android/log.h>

#define printf(...) __android_log_print(ANDROID_LOG_VERBOSE, "UI-Console", __VA_ARGS__)
#endif

#ifndef WIN32
/* Allow external modules to call the main function as a library method.  This is useful for user
* interfaces that simply layer on top of (rather than re-implement) UI-Console (e.g. for android).
*/
__attribute__((visibility("default")))
#endif
int main(int argc, char * argv[])
{
    printf("    ____               _           __  _____ __ __\n");
    printf("   / __ \\_________    (_)__  _____/ /_/ ___// // /\n");
    printf("  / /_/ / ___/ __ \\  / / _ \\/ ___/ __/ __ \\/ // /_\n");
    printf(" / ____/ /  / /_/ / / /  __/ /__/ /_/ /_/ /__  __/\n");
    printf("/_/   /_/   \\____/_/ /\\___/\\___/\\__/\\____/  /_/   \n");
    printf("                /___/                             \n");
    printf("http://www.pj64-emu.com/\n");
    printf("%s Version %s\n\n", VER_FILE_DESCRIPTION_STR, VER_FILE_VERSION_STR);

    printf("argc = %d\n", argc);
    for (int i = 0; i < argc; i++)
    {
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    printf("before app init\n");
    if (AppInit(NULL, argc, &argv[0]))
	{
		if (g_Settings->LoadStringVal(Cmd_RomFile).length() > 0)
		{
			CN64System::RunFileImage(g_Settings->LoadStringVal(Cmd_RomFile).c_str());
		}
	}
	AppCleanup();
    printf("After app init\n");

    return 0;
}