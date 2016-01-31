#include "audio_1.1.h"
#include "Version.h"
#include <stdio.h>
#include <string.h>

AUDIO_INFO g_AudioInfo;

EXPORT void CALL AiDacrateChanged(int /*SystemType*/)
{
    int a = 5;
    a = a;
}

EXPORT void CALL AiLenChanged(void)
{
    int a = 5;
    a = a;
}

EXPORT uint32_t CALL AiReadLength(void)
{
    return 0;
}

EXPORT void CALL AiUpdate(int32_t Wait)
{
    int a = 5;
    a = a;
}

EXPORT void CALL CloseDLL(void)
{
    int a = 5;
    a = a;
}

EXPORT void CALL DllAbout(void * hParent)
{
    //printf ("dummy audio plugin" );
}

EXPORT void CALL DllConfig(void * hParent)
{
}

EXPORT void CALL DllTest(void * hParent)
{
}

EXPORT void CALL GetDllInfo(PLUGIN_INFO * PluginInfo)
{
    PluginInfo->Version = 0x0101;
    PluginInfo->Type = PLUGIN_TYPE_AUDIO;
#ifdef _DEBUG
    sprintf(PluginInfo->Name, "Android Audio Debug Plugin %s", VER_FILE_VERSION_STR);
#else
    sprintf(PluginInfo->Name, "Android Audio Plugin %s", VER_FILE_VERSION_STR);
#endif
    PluginInfo->MemoryBswaped = true;
    PluginInfo->NormalMemory = false;
}

EXPORT int32_t CALL InitiateAudio(AUDIO_INFO Audio_Info)
{
    g_AudioInfo = Audio_Info;
    return true;
}

EXPORT void CALL RomOpen()
{
    int a = 5;
    a = a;
}

EXPORT void CALL RomClosed(void)
{
    int a = 5;
    a = a;
}

EXPORT void CALL ProcessAList(void)
{
    int a = 5;
    a = a;
}