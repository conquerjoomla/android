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

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import emu.project64.jni.NativeExports;

public class RendererWrapper implements GLThread.Renderer
{
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
 
    @Override
    public void onDrawFrame(GL10 gl) 
    {
    	NativeExports.onDrawFrame();
    }
}
