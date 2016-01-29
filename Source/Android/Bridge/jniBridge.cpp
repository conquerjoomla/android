/****************************************************************************
*                                                                           *
* Project 64 - A Nintendo 64 emulator.                                      *
* http://www.pj64-emu.com/                                                  *
* Copyright (C) 2012 Project64. All rights reserved.                        *
*                                                                           *
* License:                                                                  *
* GNU/GPLv2 http://www.gnu.org/licenses/gpl-2.0.html                        *
*                                                                           *
****************************************************************************/
#include <stdlib.h>
#include "NotificationClass.h"
#include <Project64-core/AppInit.h>
#include <Project64-core/Version.h>
#include <Project64-core/TraceModulesProject64.h>
#include <Project64-core/Settings/SettingsClass.h>
#include <Common/Trace.h>

#ifdef _WIN32
#define EXPORT      extern "C" __declspec(dllexport)
#define CALL        __cdecl
#else
#define EXPORT      extern "C" __attribute__((visibility("default")))
#define CALL
#endif


#ifdef ANDROID
#include <jni.h>
#include <android/log.h>

class AndroidLogger : public CTraceModule
{
    void Write(uint32_t module, uint8_t severity, const char * file, int line, const char * function, const char * Message)
    {
        switch (severity)
        {
        case TraceError: __android_log_print(ANDROID_LOG_ERROR, TraceModule(module), "%s: %s",function,Message); break;
        case TraceWarning:  __android_log_print(ANDROID_LOG_WARN, TraceModule(module), "%s: %s",function,Message); break;
        case TraceNotice: __android_log_print(ANDROID_LOG_INFO, TraceModule(module), "%s: %s",function,Message); break;
        case TraceInfo: __android_log_print(ANDROID_LOG_INFO, TraceModule(module), "%s: %s",function,Message); break;
        case TraceDebug: __android_log_print(ANDROID_LOG_DEBUG, TraceModule(module), "%s: %s",function,Message); break;
        case TraceVerbose: __android_log_print(ANDROID_LOG_VERBOSE, TraceModule(module), "%s: %s",function,Message); break;
        default: __android_log_print(ANDROID_LOG_UNKNOWN, TraceModule(module), "%s: %s",function,Message); break;
        }
    }
};

EXPORT jboolean CALL Java_emu_project64_jni_NativeExports_appInit(JNIEnv* env, jclass cls, jstring BaseDir)
{
	AndroidLogger Logger;
    TraceAddModule(&Logger);

    Notify().DisplayMessage(10, "    ____               _           __  _____ __ __");
    Notify().DisplayMessage(10, "   / __ \\_________    (_)__  _____/ /_/ ___// // /");
    Notify().DisplayMessage(10, "  / /_/ / ___/ __ \\  / / _ \\/ ___/ __/ __ \\/ // /_");
    Notify().DisplayMessage(10, " / ____/ /  / /_/ / / /  __/ /__/ /_/ /_/ /__  __/");
    Notify().DisplayMessage(10, "/_/   /_/   \\____/_/ /\\___/\\___/\\__/\\____/  /_/");
    Notify().DisplayMessage(10, "                /___/");
    Notify().DisplayMessage(10, "http://www.pj64-emu.com/");
    Notify().DisplayMessage(10, stdstr_f("%s Version %s", VER_FILE_DESCRIPTION_STR, VER_FILE_VERSION_STR).c_str());
    Notify().DisplayMessage(10, "");

	const char *baseDir = env->GetStringUTFChars(BaseDir, 0);
	bool res = AppInit(&Notify(), baseDir, 0, NULL);
    env->ReleaseStringUTFChars(BaseDir, baseDir);
	if (!res)
    {
        AppCleanup();
    }
    return res;
}


EXPORT void CALL Java_emu_project64_jni_NativeExports_SettingsSaveBool(JNIEnv* env, jclass cls, int Type, jboolean Value)
{
	WriteTrace(TraceUserInterface, TraceDebug, "Saving %d value: %s",Type,Value ? "true" : "false");	
	g_Settings->SaveBool((SettingID)Type, Value);
	WriteTrace(TraceUserInterface, TraceDebug, "Saved");	
}

/*EXPORT void CALL Java_emu_project64_jni_NativeExports_SettingsSaveDword(JNIEnv* env, jclass cls, int Type, uint32_t Value)
{
    Notify().DisplayMessage(10, "SettingsSaveDword: ");
}*/

EXPORT void CALL Java_emu_project64_jni_NativeExports_SettingsSaveString(JNIEnv* env, jclass cls, int Type, jstring Buffer)
{
	const char *value = env->GetStringUTFChars(Buffer, 0);
	WriteTrace(TraceUserInterface, TraceDebug, "Saving %d value: %s",Type,value);	
	g_Settings->SaveString((SettingID)Type, value);
	WriteTrace(TraceUserInterface, TraceDebug, "Saved");	
    env->ReleaseStringUTFChars(Buffer, value);
}

extern "C" void Java_emu_project64_jni_NativeExports_loadLibraries(JNIEnv* env, jclass cls, jstring jlibPath)
{
}

extern "C" void Java_emu_project64_jni_NativeExports_unloadLibraries(JNIEnv* env, jclass cls)
{
}

extern "C" jint Java_emu_project64_jni_NativeExports_emuStart(JNIEnv* env, jclass cls, jstring juserDataPath, jstring juserCachePath, jobjectArray jargv)
{
}
#endif
