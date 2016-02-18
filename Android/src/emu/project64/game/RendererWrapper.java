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
package emu.project64.game;

import java.lang.ref.WeakReference;

import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import emu.project64.jni.NativeExports;

public class RendererWrapper extends Thread implements GLThread.SurfaceInfo
{
    RendererWrapper(GameSurface Surface)
    {
        super();
        mGLThread = new GLThread(new WeakReference<GameSurface>(Surface), this);
        mSurface = Surface;
    }
    
    @Override
    public void run()
    {
        Log.i("RendererWrapper", "starting tid=" + getId());
        try 
        {
            mGLThread.ThreadStarting();
            while (true) 
            {
                mGLThread.ReadyToDraw();
                NativeExports.onDrawFrame();
                mGLThread.SwapBuffers();
            }
        }
        finally
        {
            mGLThread.ThreadExiting();
        }
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config)
    {
    	NativeExports.onSurfaceCreated();
    }
 
    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height)
    {
    	NativeExports.onSurfaceChanged(width, height);
    }
 
    GLThread mGLThread;
    GameSurface mSurface;

}
