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
#include "JavaBridge.h"
#include "jniBridge.h"

#ifdef ANDROID
#include <android/log.h>

JavaBridge::JavaBridge(JavaVM* vm) :
	m_vm(vm)
{
}

void JavaBridge::GfxThreadInit()
{
    __android_log_print(ANDROID_LOG_ERROR, "jniBridge", "JavaBridge::GfxThreadInit");
    JNIEnv *env = Android_JNI_GetEnv();
    __android_log_print(ANDROID_LOG_ERROR, "jniBridge", "JavaBridge::GfxThreadInit env = %p",env);
	if (g_GLThread != NULL)
	{
		jclass GLThreadClass = env->GetObjectClass(g_GLThread);
		__android_log_print(ANDROID_LOG_ERROR, "jniBridge", "JavaBridge::GfxThreadInit GLThreadClass = %p",GLThreadClass);
		jmethodID midThreadStarting = env->GetMethodID(GLThreadClass, "ThreadStarting", "()V");
		__android_log_print(ANDROID_LOG_ERROR, "jniBridge", "JavaBridge::GfxThreadInit midThreadStarting = %p",midThreadStarting);
		env->CallVoidMethod(g_GLThread, midThreadStarting);
		__android_log_print(ANDROID_LOG_ERROR, "jniBridge", "JavaBridge::GfxThreadInit Called?");
		env->DeleteLocalRef(GLThreadClass);
	}
	else
	{
		__android_log_print(ANDROID_LOG_ERROR, "jniBridge", "g_GLThread == NULL");
	}
}

void JavaBridge::GfxThreadDone()
{
    __android_log_print(ANDROID_LOG_ERROR, "jniBridge", "JavaBridge::GfxThreadDone");
}

void JavaBridge::SwapWindow()
{
    __android_log_print(ANDROID_LOG_ERROR, "jniBridge", "JavaBridge::SwapWindow");
    JNIEnv *env = Android_JNI_GetEnv();
	if (g_GLThread != NULL)
	{
		jclass GLThreadClass = env->GetObjectClass(g_GLThread);
		jmethodID midSwapBuffers = env->GetMethodID(GLThreadClass, "SwapBuffers", "()V");
		env->CallVoidMethod(g_GLThread, midSwapBuffers);
		env->DeleteLocalRef(GLThreadClass);
	}
	else
	{
		__android_log_print(ANDROID_LOG_ERROR, "jniBridge", "g_GLThread == NULL");
	}
}

#endif