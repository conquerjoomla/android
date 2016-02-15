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

import android.util.Log;

import emu.project64.jni.NativeExports;

public class RendererWrapper extends Thread
{
    RendererWrapper(GameSurface Surface)
    {
        super();
        mGLThread = new GLThread(Surface);
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

    GLThread mGLThread;
    GameSurface mSurface;
}
