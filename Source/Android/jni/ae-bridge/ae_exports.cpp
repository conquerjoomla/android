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
#include <dlfcn.h>
#include "SDL.h"
#include "ae_imports.h"

extern "C" DECLSPEC jint SDLCALL Java_emu_project64_jni_NativeExports_emuStart(JNIEnv* env, jclass cls, jstring juserDataPath, jstring juserCachePath, jobjectArray jargv)
{
	LOGI("in Java_emu_project64_jni_NativeExports_emuStart");
    return 0;
}
