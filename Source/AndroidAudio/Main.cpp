#include "audio_1.1.h"

AUDIO_INFO g_AudioInfo;

EXPORT void CALL AiDacrateChanged( int SystemType )
{
}

EXPORT void CALL AiLenChanged( void )
{
}

EXPORT DWORD CALL AiReadLength( void )
{
    return 0;
}

EXPORT void CALL AiUpdate( BOOL Wait )
{
}

EXPORT void CALL CloseDLL( void )
{
}

EXPORT void CALL DllAbout( HWND hParent )
{
    //printf ("dummy audio plugin" );
}

EXPORT void CALL DllConfig ( HWND hParent )
{
}

EXPORT void CALL DllTest ( HWND hParent )
{
}

EXPORT void CALL GetDllInfo( PLUGIN_INFO * PluginInfo )
{
    PluginInfo->Version = 0x0101;
    PluginInfo->Type    = PLUGIN_TYPE_AUDIO;
    sprintf(PluginInfo->Name,"Android Audio");
    PluginInfo->NormalMemory  = TRUE;
    PluginInfo->MemoryBswaped = TRUE;
}

EXPORT BOOL CALL InitiateAudio( AUDIO_INFO Audio_Info )
{
    g_AudioInfo = Audio_Info;
    return TRUE;
}

EXPORT void CALL RomOpen()
{
}

EXPORT void CALL RomClosed( void )
{
}

EXPORT void CALL ProcessAlist( void )
{
}