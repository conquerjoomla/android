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

import android.opengl.GLSurfaceView;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class RendererWrapper implements GLSurfaceView.Renderer
{
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config)
    {
    	gl.glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
    }
 
    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height)
    {
        // No-op
    }
 
    @Override
    public void onDrawFrame(GL10 gl) 
    {
    	gl.glClear(gl.GL_COLOR_BUFFER_BIT);
    }
}
