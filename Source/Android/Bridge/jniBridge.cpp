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
#include <Project64-core/N64System/N64Class.h>
#include <Project64-core/N64System/SystemGlobals.h>
#include <Project64-core/Plugin.h>
#include <Common/Trace.h>
#include "jniBridge.h"
#include "jniBridgeSettings.h"
#include "JavaBridge.h"

#ifdef _WIN32
#define EXPORT      extern "C" __declspec(dllexport)
#define CALL        __cdecl
#else
#define EXPORT      extern "C" __attribute__((visibility("default")))
#define CALL
#endif

CJniBridegSettings * JniBridegSettings = NULL;

#ifdef ANDROID
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
AndroidLogger * g_Logger = NULL;
static pthread_key_t g_ThreadKey;
static JavaVM* g_JavaVM = NULL;
static JavaBridge * g_JavaBridge = NULL;
jobject g_GLThread = NULL;

static void Android_JNI_ThreadDestroyed(void*);
static void Android_JNI_SetupThread(void);

EXPORT jint CALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    __android_log_print(ANDROID_LOG_INFO, "jniBridge", "JNI_OnLoad called");
    g_JavaVM = vm;

    JNIEnv *env;
    if (g_JavaVM->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK)
    {
        __android_log_print(ANDROID_LOG_ERROR, "jniBridge", "Failed to get the environment using GetEnv()");
        return -1;
    }
    /*
    * Create mThreadKey so we can keep track of the JNIEnv assigned to each thread
    * Refer to http://developer.android.com/guide/practices/design/jni.html for the rationale behind this
    */
    if (pthread_key_create(&g_ThreadKey, Android_JNI_ThreadDestroyed) != 0)
    {
        __android_log_print(ANDROID_LOG_ERROR, "jniBridge", "Error initializing pthread key");
    }
    Android_JNI_SetupThread();
    return JNI_VERSION_1_4;
}

EXPORT jboolean CALL Java_emu_project64_jni_NativeExports_appInit(JNIEnv* env, jclass cls, jstring BaseDir)
{
    if (g_Logger == NULL)
    {
        g_Logger = new AndroidLogger();
    }
    TraceAddModule(g_Logger);

    Notify().DisplayMessage(10, "    ____               _           __  _____ __ __");
    Notify().DisplayMessage(10, "   / __ \\_________    (_)__  _____/ /_/ ___// // /");
    Notify().DisplayMessage(10, "  / /_/ / ___/ __ \\  / / _ \\/ ___/ __/ __ \\/ // /_");
    Notify().DisplayMessage(10, " / ____/ /  / /_/ / / /  __/ /__/ /_/ /_/ /__  __/");
    Notify().DisplayMessage(10, "/_/   /_/   \\____/_/ /\\___/\\___/\\__/\\____/  /_/");
    Notify().DisplayMessage(10, "                /___/");
    Notify().DisplayMessage(10, "http://www.pj64-emu.com/");
    Notify().DisplayMessage(10, stdstr_f("%s Version %s", VER_FILE_DESCRIPTION_STR, VER_FILE_VERSION_STR).c_str());
    Notify().DisplayMessage(10, "");

    if (g_JavaVM == NULL)
    {
        Notify().DisplayError("No java VM");
        return false;
    }

    const char *baseDir = env->GetStringUTFChars(BaseDir, 0);
    bool res = AppInit(&Notify(), baseDir, 0, NULL);
    env->ReleaseStringUTFChars(BaseDir, baseDir);
    if (res)
    {
        g_JavaBridge = new JavaBridge(g_JavaVM);
        g_Plugins->SetRenderWindows(g_JavaBridge, NULL);

        JniBridegSettings = new CJniBridegSettings();
    }
    else
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

#include <GLES2/gl2.h>
void UITestThread(void * param)
{
    WriteTrace(TraceN64System, TraceDebug, "Start");
    g_JavaBridge->GfxThreadInit();
    float red = 1.0f;
    bool down = true;
    for(;;)
    {
        WriteTrace(TraceN64System, TraceDebug, "red = %f",red);
        glClearColor(red, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        red += down ? - 0.01 : +0.01;
        if (red < 0.1f)
        {
            down = false;
        }
        else if (red > 0.9f)
        {
            down = true;
        }
        g_JavaBridge->SwapWindow();
    }
}

EXPORT jint CALL Java_emu_project64_jni_NativeExports_RunFileImage(JNIEnv* env, jclass cls, jstring FileLoc, jobject GLThread)
{
    g_GLThread = env->NewGlobalRef(GLThread);
    const char *fileLoc = env->GetStringUTFChars(FileLoc, 0);
    WriteTrace(TraceUserInterface, TraceDebug, "FileLoc: %s",fileLoc);
    CN64System::RunFileImage(fileLoc);
    {
        //    pthread_t * thread_id = new pthread_t;
        //    int res = pthread_create(thread_id, NULL, (void *(*)(void *))UITestThread, NULL);
    }
    env->ReleaseStringUTFChars(FileLoc, fileLoc);
    WriteTrace(TraceUserInterface, TraceDebug, "Image started");
}

EXPORT jint CALL Java_emu_project64_jni_NativeExports_emuGetState(JNIEnv* env, jclass cls)
{
    WriteTrace(TraceUserInterface, TraceDebug, "Start");
    if (JniBridegSettings != NULL)
    {
        //need to add a test for paused(3)
        if (!JniBridegSettings->bCPURunning())
        {
            WriteTrace(TraceUserInterface, TraceDebug, "Done (res: 1) CPU not running");
            return (jint) 1; // CPU not running
        }
        if (JniBridegSettings->bCPURunning())
        {
            WriteTrace(TraceUserInterface, TraceDebug, "Done (res: 2) CPU is running");
            return (jint) 2; // CPU is running
        }
    }
    WriteTrace(TraceUserInterface, TraceDebug, "Done (res: 0)  Unknown - JniBridegSettings does not exist");
    return (jint)0; // Unknown
}

static void Android_JNI_ThreadDestroyed(void* value)
{
    /* The thread is being destroyed, detach it from the Java VM and set the mThreadKey value to NULL as required */
    JNIEnv *env = (JNIEnv*)value;
    if (env != NULL)
    {
        g_JavaVM->DetachCurrentThread();
        pthread_setspecific(g_ThreadKey, NULL);
    }
}

JNIEnv* Android_JNI_GetEnv(void)
{
    /* From http://developer.android.com/guide/practices/jni.html
    * All threads are Linux threads, scheduled by the kernel.
    * They're usually started from managed code (using Thread.start), but they can also be created elsewhere and then
    * attached to the JavaVM. For example, a thread started with pthread_create can be attached with the
    * JNI AttachCurrentThread or AttachCurrentThreadAsDaemon functions. Until a thread is attached, it has no JNIEnv,
    * and cannot make JNI calls.
    * Attaching a natively-created thread causes a java.lang.Thread object to be constructed and added to the "main"
    * ThreadGroup, making it visible to the debugger. Calling AttachCurrentThread on an already-attached thread
    * is a no-op.
    * Note: You can call this function any number of times for the same thread, there's no harm in it
    */

    JNIEnv *env;
    int status = g_JavaVM->AttachCurrentThread(&env, NULL);
    if (status < 0)
    {
        __android_log_print(ANDROID_LOG_ERROR, "jniBridge", "failed to attach current thread");
        return 0;
    }

    /* From http://developer.android.com/guide/practices/jni.html
    * Threads attached through JNI must call DetachCurrentThread before they exit. If coding this directly is awkward,
    * in Android 2.0 (Eclair) and higher you can use pthread_key_create to define a destructor function that will be
    * called before the thread exits, and call DetachCurrentThread from there. (Use that key with pthread_setspecific
    * to store the JNIEnv in thread-local-storage; that way it'll be passed into your destructor as the argument.)
    * Note: The destructor is not called unless the stored value is != NULL
    * Note: You can call this function any number of times for the same thread, there's no harm in it
    *       (except for some lost CPU cycles)
    */
    pthread_setspecific(g_ThreadKey, (void*)env);
    return env;
}

void Android_JNI_SetupThread(void)
{
    Android_JNI_GetEnv();
}

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#define  LOG_TAG    "libgl2jni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
        = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

static const char gVertexShader[] =
"attribute vec4 vPosition;\n"
"void main() {\n"
"  gl_Position = vPosition;\n"
"}\n";

static const char gFragmentShader[] =
"precision mediump float;\n"
"void main() {\n"
"  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
"}\n";

GLuint loadShader(GLenum shaderType, const char* pSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*)malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE("Could not compile shader %d:\n%s\n",
                        shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

GLuint createProgram(const char* pVertexSource, const char* pFragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
        checkGlError("glAttachShader");
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*)malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

GLuint gProgram;
GLuint gvPositionHandle;

bool setupGraphics(int w, int h) {
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);

    LOGI("setupGraphics(%d, %d)", w, h);
    gProgram = createProgram(gVertexShader, gFragmentShader);
    if (!gProgram) {
        LOGE("Could not create program.");
        return false;
    }
    gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");
    checkGlError("glGetAttribLocation");
    LOGI("glGetAttribLocation(\"vPosition\") = %d\n",
        gvPositionHandle);

    glViewport(0, 0, w, h);
    checkGlError("glViewport");
    return true;
}

const GLfloat gTriangleVertices[] = { 0.0f, 0.5f, -0.5f, -0.5f,
0.5f, -0.5f };

void renderFrame() {
    static float grey;
    grey += 0.01f;
    if (grey > 1.0f) {
        grey = 0.0f;
    }
    glClearColor(grey, grey, grey, 1.0f);
    checkGlError("glClearColor");
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    checkGlError("glClear");

    glUseProgram(gProgram);
    checkGlError("glUseProgram");

    glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, gTriangleVertices);
    checkGlError("glVertexAttribPointer");
    glEnableVertexAttribArray(gvPositionHandle);
    checkGlError("glEnableVertexAttribArray");
    glDrawArrays(GL_TRIANGLES, 0, 3);
    checkGlError("glDrawArrays");
}

EXPORT void CALL Java_emu_project64_jni_NativeExports_onSurfaceCreated(JNIEnv * env, jclass cls)
{
}

EXPORT void CALL Java_emu_project64_jni_NativeExports_onSurfaceChanged(JNIEnv * env, jclass cls, jint width, jint height)
{
    setupGraphics(width, height);
}

EXPORT void CALL Java_emu_project64_jni_NativeExports_onDrawFrame(JNIEnv * env, jclass cls)
{
    renderFrame();
}

#endif