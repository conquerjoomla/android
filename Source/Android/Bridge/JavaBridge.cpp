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
JavaBridge::JavaBridge(JavaVM* vm) :
	m_vm(vm)
{
}

void JavaBridge::GfxThreadInit()
{
    JNIEnv *env = Android_JNI_GetEnv();
	if (g_GLThread != NULL)
	{
		jclass GLThreadClass = env->GetObjectClass(g_GLThread);
		jmethodID midThreadStarting = env->GetMethodID(GLThreadClass, "ThreadStarting", "()V");
		env->CallVoidMethod(g_GLThread, midThreadStarting);
		env->DeleteLocalRef(GLThreadClass);
	}
}

void JavaBridge::GfxThreadDone()
{
}

void JavaBridge::SwapWindow()
{
    JNIEnv *env = Android_JNI_GetEnv();
	if (g_GLThread != NULL)
	{
		jclass GLThreadClass = env->GetObjectClass(g_GLThread);
		jmethodID midSwapBuffers = env->GetMethodID(GLThreadClass, "SwapBuffers", "()V");
		env->CallVoidMethod(g_GLThread, midSwapBuffers);
		env->DeleteLocalRef(GLThreadClass);
	}
}

#endif