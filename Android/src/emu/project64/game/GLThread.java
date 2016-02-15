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

import java.util.ArrayList;
import android.util.Log;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGL11;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;
import javax.microedition.khronos.opengles.GL;
import javax.microedition.khronos.opengles.GL10;

/**
 * A generic GL Thread. Takes care of initializing EGL and GL. Delegates
 * to a Renderer instance to do the actual drawing. Can be configured to
 * render continuously or on request.
 *
 */
public class GLThread
{
    GLThread(GameSurface Surface )
    {
        super();
        mDone = false;
        mSurface = Surface;
    }

    public void ThreadStarting() 
    {
        try {
            mSurface.sEglSemaphore.acquire();
            mSurface.mEglHelper.start();
            ReadyToDraw();
        } catch (InterruptedException e) {
            return;
        }
    }

    public void ThreadExiting()
    {
        /*
         * clean-up everything...
         */
        mSurface.sEglSemaphore.release();
    }

    public void ReadyToDraw() 
    {
       // boolean tellRendererSurfaceCreated = true;
       // boolean tellRendererSurfaceChanged = true;
    	
        /*
         *  Update the asynchronous state (window size)
         */
        //int w, h;
        boolean changed = false;
        synchronized (this)
        {
            Runnable r;
            while ((r = getEvent()) != null) 
            {
                r.run();
            }
            while (mSurface.needToWait()) 
            {
                if (!mHasSurface) 
                {
                    if (!mWaitingForSurface) 
                    {
                        mSurface.mEglHelper.destroySurface();
                        mWaitingForSurface = true;
                        notify();
                    }
                }        
                try 
                {
                	wait();
                } 
                catch (InterruptedException e) 
                {
                    return;
                }
            }
            if (mDone) 
            {
                return;
            }
            changed = mSizeChanged;
            //w = mSurface.mWidth;
            //h = mSurface.mHeight;
            mSizeChanged = false;
            if (mHasSurface && mWaitingForSurface) {
                changed = true;
                mWaitingForSurface = false;
            }
        }
        if (changed) 
        {
        	mgl = (GL10) mSurface.mEglHelper.createSurface();
            //tellRendererSurfaceChanged = true;
        }
        /*if (tellRendererSurfaceCreated) {
            mRenderer.onSurfaceCreated(gl, mSurface.mEglHelper.mEglConfig);
            tellRendererSurfaceCreated = false;
        }
        if (tellRendererSurfaceChanged) {
            mRenderer.onSurfaceChanged(gl, w, h);
            tellRendererSurfaceChanged = false;
        }*/
    }

    public void SwapBuffers()
    {
        Log.i("GLThread", "SwapBuffers()");
        /*
         * Once we're done with GL, we need to call swapBuffers()
         * to instruct the system to display the rendered frame
         */
        mSurface.mEglHelper.swap();
    }

    public void surfaceCreated() 
    {
        synchronized(this) 
        {
            mHasSurface = true;
            notify();
        }
    }

    public void surfaceDestroyed() 
    {
        synchronized(this) 
        {
            mHasSurface = false;
            notify();
            while(!mWaitingForSurface) 
            {
                try 
                {
                    wait();
                }
                catch (InterruptedException e) 
                {
                    Thread.currentThread().interrupt();
                }
            }
        }
    }

    /**
     * Queue an "event" to be run on the GL rendering thread.
     * @param r the runnable to be run on the GL rendering thread.
     */
    public void queueEvent(Runnable r) 
    {
        synchronized(this) 
        {
            mEventQueue.add(r);
        }
    }

    private Runnable getEvent() 
    {
        synchronized(this) 
        {
            if (mEventQueue.size() > 0) 
            {
                return mEventQueue.remove(0);
            }
        }
        return null;
    }

    private GL10 mgl = null;
    private GameSurface mSurface;
    private boolean mDone;
    private boolean mHasSurface = false;
    public boolean mWaitingForSurface;
    private ArrayList<Runnable> mEventQueue = new ArrayList<Runnable>();
    private boolean mSizeChanged = true;
}
