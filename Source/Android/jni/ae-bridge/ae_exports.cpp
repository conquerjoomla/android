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

static void *handleFront;    // libmupen64plus-ui-console.so

typedef int         (*pFrontMain)       (int argc, char* argv[]);

// Function pointers
static pFrontMain       frontMain       = NULL;

void checkLibraryError(const char* message)
{
    const char* error = dlerror();
    if (error)
        LOGE("%s: %s", message, error);
}

void* loadLibrary(const char* libPath, const char* libName)
{
    char path[256];
    sprintf(path, "%s/lib%s.so", libPath, libName);
    void* handle = dlopen(path, RTLD_NOW);
    if (!handle)
        LOGE("Failed to load lib%s.so", libName);
    checkLibraryError(libName);
    return handle;
}


void* locateFunction(void* handle, const char* libName, const char* funcName)
{
    char message[256];
    sprintf(message, "Locating %s::%s", libName, funcName);
    void* func = dlsym(handle, funcName);
    if (!func)
        LOGE("Failed to locate %s::%s", libName, funcName);
    checkLibraryError(message);
    return func;
}
extern "C" DECLSPEC void SDLCALL Java_emu_project64_jni_NativeExports_loadLibraries(JNIEnv* env, jclass cls, jstring jlibPath)
{
    LOGI("Loading native libraries");

    // Get the library path from the java-managed string
    const char *libPath = env->GetStringUTFChars(jlibPath, 0);
    char path[256];
    strcpy(path, libPath);
    env->ReleaseStringUTFChars(jlibPath, libPath);

    // Open shared libraries
    handleFront    = loadLibrary(path, "mupen64plus-ui-console");

    // Make sure we don't have any typos
    if (!handleFront )
    {
        LOGE("Could not load libraries: be sure the paths are correct");
    }

    frontMain     = (pFrontMain)     locateFunction(handleFront, "mupen64plus-ui-console", "SDL_main");
    // Make sure we don't have any typos
    if (!frontMain)
    {
        LOGE("Could not load library functions: be sure they are named and typedef'd correctly");
    }
}

extern "C" DECLSPEC void SDLCALL Java_emu_project64_jni_NativeExports_unloadLibraries(JNIEnv* env, jclass cls)
{
    // Unload the libraries to ensure that static variables are re-initialized next time
    LOGI("Unloading native libraries");

    frontMain       = NULL;
}

extern "C" DECLSPEC jint SDLCALL Java_emu_project64_jni_NativeExports_emuStart(JNIEnv* env, jclass cls, jstring juserDataPath, jstring juserCachePath, jobjectArray jargv)
{
    // Repackage the command-line args
    int argc = env->GetArrayLength(jargv);
    char **argv = (char **) malloc(sizeof(char *) * argc);
    for (int i = 0; i < argc; i++)
    {
        jstring jarg = (jstring) env->GetObjectArrayElement(jargv, i);
        const char *arg = env->GetStringUTFChars(jarg, 0);
        argv[i] = strdup(arg);
        env->ReleaseStringUTFChars(jarg, arg);
    }

    // Launch main emulator loop (continues until emuStop is called)
    return frontMain(argc, argv);
}
