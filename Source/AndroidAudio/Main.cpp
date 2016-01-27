#include "audio_1.1.h"
#include <string>

AUDIO_INFO g_AudioInfo;

EXPORT void CALL AiDacrateChanged(int /*SystemType*/)
{
}

EXPORT void CALL AiLenChanged(void)
{
}

EXPORT uint32_t CALL AiReadLength(void)
{
    return 0;
}

EXPORT void CALL AiUpdate(int32_t Wait)
{
}

EXPORT void CALL CloseDLL(void)
{
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
    strcpy(PluginInfo->Name, "Android Audio");
}

EXPORT int32_t CALL InitiateAudio(AUDIO_INFO Audio_Info)
{
    g_AudioInfo = Audio_Info;
    return true;
}

EXPORT void CALL RomOpen()
{
}

EXPORT void CALL RomClosed(void)
{
}

EXPORT void CALL ProcessAlist(void)
{
}