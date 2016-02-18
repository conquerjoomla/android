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

import java.io.Writer;
import java.lang.ref.WeakReference;
import java.util.ArrayList;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGL11;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;
import javax.microedition.khronos.opengles.GL;
import javax.microedition.khronos.opengles.GL10;

import android.content.pm.ConfigurationInfo;
import android.util.Log;
import emu.project64.game.GameSurface.EGLContextFactory;
import emu.project64.game.GameSurface.LogWriter;

/**
 * A generic GL Thread. Takes care of initializing EGL and GL. Delegates
 * to a Renderer instance to do the actual drawing. Can be configured to
 * render continuously or on request.
 *
 * All potentially blocking synchronization is done through the
 * sGLThreadManager object. This avoids multiple-lock ordering issues.
 *
 */
class GLThread extends Thread 
{
	private final static boolean LOG_THREADS = false;
    private final static boolean LOG_SURFACE = false;
    private final static boolean LOG_EGL = false;
    private final static boolean LOG_PAUSE_RESUME = false;
    private final static boolean LOG_RENDERER = false;
    private final static boolean LOG_RENDERER_DRAW_FRAME = false;


    /**
     * Check glError() after every GL call and throw an exception if glError indicates
     * that an error has occurred. This can be used to help track down which OpenGL ES call
     * is causing an error.
     *
     * @see #getDebugFlags
     * @see #setDebugFlags
     */
    public final static int DEBUG_CHECK_GL_ERROR = 1;
    
    /**
     * Log GL calls to the system log at "verbose" level with tag "GLSurfaceView".
     *
     * @see #getDebugFlags
     * @see #setDebugFlags
     */
    public final static int DEBUG_LOG_GL_CALLS = 2;
    
    /**
     * The renderer only renders
     * when the surface is created, or when {@link #requestRender} is called.
     *
     * @see #getRenderMode()
     * @see #setRenderMode(int)
     * @see #requestRender()
     */
    public final static int RENDERMODE_WHEN_DIRTY = 0;
    /**
     * The renderer is called
     * continuously to re-render the scene.
     *
     * @see #getRenderMode()
     * @see #setRenderMode(int)
     */
    public final static int RENDERMODE_CONTINUOUSLY = 1;

    /**
     * A generic renderer interface.
     * <p>
     * The renderer is responsible for making OpenGL calls to render a frame.
     * <p>
     * GLSurfaceView clients typically create their own classes that implement
     * this interface, and then call {@link GLSurfaceView#setRenderer} to
     * register the renderer with the GLSurfaceView.
     * <p>
     *
     * <div class="special reference">
     * <h3>Developer Guides</h3>
     * <p>For more information about how to use OpenGL, read the
     * <a href="{@docRoot}guide/topics/graphics/opengl.html">OpenGL</a> developer guide.</p>
     * </div>
     *
     * <h3>Threading</h3>
     * The renderer will be called on a separate thread, so that rendering
     * performance is decoupled from the UI thread. Clients typically need to
     * communicate with the renderer from the UI thread, because that's where
     * input events are received. Clients can communicate using any of the
     * standard Java techniques for cross-thread communication, or they can
     * use the {@link GLSurfaceView#queueEvent(Runnable)} convenience method.
     * <p>
     * <h3>EGL Context Lost</h3>
     * There are situations where the EGL rendering context will be lost. This
     * typically happens when device wakes up after going to sleep. When
     * the EGL context is lost, all OpenGL resources (such as textures) that are
     * associated with that context will be automatically deleted. In order to
     * keep rendering correctly, a renderer must recreate any lost resources
     * that it still needs. The {@link #onSurfaceCreated(GL10, EGLConfig)} method
     * is a convenient place to do this.
     *
     *
     * @see #setRenderer(Renderer)
     */
    public interface Renderer {
        /**
         * Called when the surface is created or recreated.
         * <p>
         * Called when the rendering thread
         * starts and whenever the EGL context is lost. The EGL context will typically
         * be lost when the Android device awakes after going to sleep.
         * <p>
         * Since this method is called at the beginning of rendering, as well as
         * every time the EGL context is lost, this method is a convenient place to put
         * code to create resources that need to be created when the rendering
         * starts, and that need to be recreated when the EGL context is lost.
         * Textures are an example of a resource that you might want to create
         * here.
         * <p>
         * Note that when the EGL context is lost, all OpenGL resources associated
         * with that context will be automatically deleted. You do not need to call
         * the corresponding "glDelete" methods such as glDeleteTextures to
         * manually delete these lost resources.
         * <p>
         * @param gl the GL interface. Use <code>instanceof</code> to
         * test if the interface supports GL11 or higher interfaces.
         * @param config the EGLConfig of the created surface. Can be used
         * to create matching pbuffers.
         */
        void onSurfaceCreated(GL10 gl, EGLConfig config);

        /**
         * Called when the surface changed size.
         * <p>
         * Called after the surface is created and whenever
         * the OpenGL ES surface size changes.
         * <p>
         * Typically you will set your viewport here. If your camera
         * is fixed then you could also set your projection matrix here:
         * <pre class="prettyprint">
         * void onSurfaceChanged(GL10 gl, int width, int height) {
         *     gl.glViewport(0, 0, width, height);
         *     // for a fixed camera, set the projection too
         *     float ratio = (float) width / height;
         *     gl.glMatrixMode(GL10.GL_PROJECTION);
         *     gl.glLoadIdentity();
         *     gl.glFrustumf(-ratio, ratio, -1, 1, 1, 10);
         * }
         * </pre>
         * @param gl the GL interface. Use <code>instanceof</code> to
         * test if the interface supports GL11 or higher interfaces.
         * @param width
         * @param height
         */
        void onSurfaceChanged(GL10 gl, int width, int height);

        /**
         * Called to draw the current frame.
         * <p>
         * This method is responsible for drawing the current frame.
         * <p>
         * The implementation of this method typically looks like this:
         * <pre class="prettyprint">
         * void onDrawFrame(GL10 gl) {
         *     gl.glClear(GL10.GL_COLOR_BUFFER_BIT | GL10.GL_DEPTH_BUFFER_BIT);
         *     //... other gl calls to render the scene ...
         * }
         * </pre>
         * @param gl the GL interface. Use <code>instanceof</code> to
         * test if the interface supports GL11 or higher interfaces.
         */
        void onDrawFrame(GL10 gl);
    }

    /**
     * An EGL helper class.
     */
    private static class EglHelper {
        public EglHelper(WeakReference<GameSurface> glSurfaceViewWeakRef) {
            mGLSurfaceViewWeakRef = glSurfaceViewWeakRef;
        }

        /**
         * Initialize EGL for a given configuration spec.
         * @param configSpec
         */
        public void start() {
            if (LOG_EGL) {
                Log.w("EglHelper", "start() tid=" + Thread.currentThread().getId());
            }
            /*
             * Get an EGL instance
             */
            mEgl = (EGL10) EGLContext.getEGL();

            /*
             * Get to the default display.
             */
            mEglDisplay = mEgl.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY);

            if (mEglDisplay == EGL10.EGL_NO_DISPLAY) {
                throw new RuntimeException("eglGetDisplay failed");
            }

            /*
             * We can now initialize EGL for that display
             */
            int[] version = new int[2];
            if(!mEgl.eglInitialize(mEglDisplay, version)) {
                throw new RuntimeException("eglInitialize failed");
            }
            GameSurface view = mGLSurfaceViewWeakRef.get();
            if (view == null) {
                mEglConfig = null;
                mEglContext = null;
            } else {
                mEglConfig = view.mEGLConfigChooser.chooseConfig(mEgl, mEglDisplay);

                /*
                * Create an EGL context. We want to do this as rarely as we can, because an
                * EGL context is a somewhat heavy object.
                */
                mEglContext = view.mEGLContextFactory.createContext(mEgl, mEglDisplay, mEglConfig);
            }
            if (mEglContext == null || mEglContext == EGL10.EGL_NO_CONTEXT) {
                mEglContext = null;
                throwEglException("createContext");
            }
            if (LOG_EGL) {
                Log.w("EglHelper", "createContext " + mEglContext + " tid=" + Thread.currentThread().getId());
            }

            mEglSurface = null;
        }

        /**
         * Create an egl surface for the current SurfaceHolder surface. If a surface
         * already exists, destroy it before creating the new surface.
         *
         * @return true if the surface was created successfully.
         */
        public boolean createSurface() {
            if (LOG_EGL) {
                Log.w("EglHelper", "createSurface()  tid=" + Thread.currentThread().getId());
            }
            /*
             * Check preconditions.
             */
            if (mEgl == null) {
                throw new RuntimeException("egl not initialized");
            }
            if (mEglDisplay == null) {
                throw new RuntimeException("eglDisplay not initialized");
            }
            if (mEglConfig == null) {
                throw new RuntimeException("mEglConfig not initialized");
            }

            /*
             *  The window size has changed, so we need to create a new
             *  surface.
             */
            destroySurfaceImp();

            /*
             * Create an EGL surface we can render into.
             */
            GameSurface view = mGLSurfaceViewWeakRef.get();
            if (view != null) {
                mEglSurface = view.mEGLWindowSurfaceFactory.createWindowSurface(mEgl,
                        mEglDisplay, mEglConfig, view.getHolder());
            } else {
                mEglSurface = null;
            }

            if (mEglSurface == null || mEglSurface == EGL10.EGL_NO_SURFACE) {
                int error = mEgl.eglGetError();
                if (error == EGL10.EGL_BAD_NATIVE_WINDOW) {
                    Log.e("EglHelper", "createWindowSurface returned EGL_BAD_NATIVE_WINDOW.");
                }
                return false;
            }

            /*
             * Before we can issue GL commands, we need to make sure
             * the context is current and bound to a surface.
             */
            if (!mEgl.eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext)) {
                /*
                 * Could not make the context current, probably because the underlying
                 * SurfaceView surface has been destroyed.
                 */
                logEglErrorAsWarning("EGLHelper", "eglMakeCurrent", mEgl.eglGetError());
                return false;
            }

            return true;
        }

        /**
         * Create a GL object for the current EGL context.
         * @return
         */
        GL createGL() {

            GL gl = mEglContext.getGL();
            GameSurface view = mGLSurfaceViewWeakRef.get();
            if (view != null) {
                if (view.mGLWrapper != null) {
                    gl = view.mGLWrapper.wrap(gl);
                }

                if ((view.mDebugFlags & (DEBUG_CHECK_GL_ERROR | DEBUG_LOG_GL_CALLS)) != 0) {
                    int configFlags = 0;
                    Writer log = null;
                    if ((view.mDebugFlags & DEBUG_CHECK_GL_ERROR) != 0) {
                        configFlags |= GLDebugHelper.CONFIG_CHECK_GL_ERROR;
                    }
                    if ((view.mDebugFlags & DEBUG_LOG_GL_CALLS) != 0) {
                        log = new LogWriter();
                    }
                    gl = GLDebugHelper.wrap(gl, configFlags, log);
                }
            }
            return gl;
        }

        /**
         * Display the current render surface.
         * @return the EGL error code from eglSwapBuffers.
         */
        public int swap() {
            if (! mEgl.eglSwapBuffers(mEglDisplay, mEglSurface)) {
                return mEgl.eglGetError();
            }
            return EGL10.EGL_SUCCESS;
        }

        public void destroySurface() {
            if (LOG_EGL) {
                Log.w("EglHelper", "destroySurface()  tid=" + Thread.currentThread().getId());
            }
            destroySurfaceImp();
        }

        private void destroySurfaceImp() {
            if (mEglSurface != null && mEglSurface != EGL10.EGL_NO_SURFACE) {
                mEgl.eglMakeCurrent(mEglDisplay, EGL10.EGL_NO_SURFACE,
                        EGL10.EGL_NO_SURFACE,
                        EGL10.EGL_NO_CONTEXT);
                GameSurface view = mGLSurfaceViewWeakRef.get();
                if (view != null) {
                    view.mEGLWindowSurfaceFactory.destroySurface(mEgl, mEglDisplay, mEglSurface);
                }
                mEglSurface = null;
            }
        }

        public void finish() {
            if (LOG_EGL) {
                Log.w("EglHelper", "finish() tid=" + Thread.currentThread().getId());
            }
            if (mEglContext != null) {
                GameSurface view = mGLSurfaceViewWeakRef.get();
                if (view != null) {
                    view.mEGLContextFactory.destroyContext(mEgl, mEglDisplay, mEglContext);
                }
                mEglContext = null;
            }
            if (mEglDisplay != null) {
                mEgl.eglTerminate(mEglDisplay);
                mEglDisplay = null;
            }
        }

        private void throwEglException(String function) {
            throwEglException(function, mEgl.eglGetError());
        }

        public static void throwEglException(String function, int error) {
            String message = formatEglError(function, error);
            if (LOG_THREADS) {
                Log.e("EglHelper", "throwEglException tid=" + Thread.currentThread().getId() + " "
                        + message);
            }
            throw new RuntimeException(message);
        }

        public static void logEglErrorAsWarning(String tag, String function, int error) {
            Log.w(tag, formatEglError(function, error));
        }

        public static String formatEglError(String function, int error) {
            return function + " failed: " + EGLLogWrapper.getErrorString(error);
        }

        private WeakReference<GameSurface> mGLSurfaceViewWeakRef;
        EGL10 mEgl;
        EGLDisplay mEglDisplay;
        EGLSurface mEglSurface;
        EGLConfig mEglConfig;
        EGLContext mEglContext;

    }

    GLThread(WeakReference<GameSurface> glSurfaceViewWeakRef, Renderer renderer) {
        super();
        glSurfaceViewWeakRef.get().mRequestRender = true;
        mRenderMode = RENDERMODE_CONTINUOUSLY;
		mRenderer = renderer;
        mGLSurfaceViewWeakRef = glSurfaceViewWeakRef;
    }

    @Override
    public void run() {
        setName("GLThread " + getId());
        if (LOG_THREADS) {
            Log.i("GLThread", "starting tid=" + getId());
        }

        try {
            guardedRun();
        } catch (InterruptedException e) {
            // fall thru and exit normally
        } finally {
            mGLSurfaceViewWeakRef.get().sGLThreadManager.threadExiting(this);
        }
    }

    /*
     * This private method should only be called inside a
     * synchronized(sGLThreadManager) block.
     */
    private void stopEglSurfaceLocked() 
    {
        GameSurface view = mGLSurfaceViewWeakRef.get();
        if (view.mHaveEglSurface) 
        {
        	view.mHaveEglSurface = false;
            mEglHelper.destroySurface();
        }
    }

    /*
     * This private method should only be called inside a
     * synchronized(sGLThreadManager) block.
     */
    private void stopEglContextLocked() 
    {
        GameSurface view = mGLSurfaceViewWeakRef.get();
        if (view.mHaveEglContext) {
            mEglHelper.finish();
            view.mHaveEglContext = false;
            mGLSurfaceViewWeakRef.get().sGLThreadManager.releaseEglContextLocked(this);
        }
    }
    private void guardedRun() throws InterruptedException {
        GameSurface view = mGLSurfaceViewWeakRef.get();
        mEglHelper = new EglHelper(mGLSurfaceViewWeakRef);
        view.mHaveEglContext = false;
        view.mHaveEglSurface = false;
        try {
            GL10 gl = null;
            boolean createEglContext = false;
            boolean createEglSurface = false;
            boolean createGlInterface = false;
            boolean lostEglContext = false;
            boolean sizeChanged = false;
            boolean wantRenderNotification = false;
            boolean doRenderNotification = false;
            boolean askedToReleaseEglContext = false;
            int w = 0;
            int h = 0;
            Runnable event = null;

            while (true) {
                synchronized (view.sGLThreadManager) {
                    while (true) {
                        if (mShouldExit) {
                            return;
                        }

                        if (! mEventQueue.isEmpty()) {
                            event = mEventQueue.remove(0);
                            break;
                        }

                        // Update the pause state.
                        boolean pausing = false;
                        if (view.mPaused != mRequestPaused) {
                            pausing = mRequestPaused;
                            view.mPaused = mRequestPaused;
                            view.sGLThreadManager.notifyAll();
                            if (LOG_PAUSE_RESUME) {
                                Log.i("GLThread", "mPaused is now " + view.mPaused + " tid=" + getId());
                            }
                        }

                        // Do we need to give up the EGL context?
                        if (mShouldReleaseEglContext) {
                            if (LOG_SURFACE) {
                                Log.i("GLThread", "releasing EGL context because asked to tid=" + getId());
                            }
                            stopEglSurfaceLocked();
                            stopEglContextLocked();
                            mShouldReleaseEglContext = false;
                            askedToReleaseEglContext = true;
                        }

                        // Have we lost the EGL context?
                        if (lostEglContext) {
                            stopEglSurfaceLocked();
                            stopEglContextLocked();
                            lostEglContext = false;
                        }

                        // When pausing, release the EGL surface:
                        if (pausing && view.mHaveEglSurface) {
                            if (LOG_SURFACE) {
                                Log.i("GLThread", "releasing EGL surface because paused tid=" + getId());
                            }
                            stopEglSurfaceLocked();
                        }

                        // When pausing, optionally release the EGL Context:
                        if (pausing && view.mHaveEglContext) {
                            boolean preserveEglContextOnPause = view == null ?
                                    false : view.mPreserveEGLContextOnPause;
                            if (!preserveEglContextOnPause || view.sGLThreadManager.shouldReleaseEGLContextWhenPausing()) {
                                stopEglContextLocked();
                                if (LOG_SURFACE) {
                                    Log.i("GLThread", "releasing EGL context because paused tid=" + getId());
                                }
                            }
                        }

                        // When pausing, optionally terminate EGL:
                        if (pausing) {
                            if (view.sGLThreadManager.shouldTerminateEGLWhenPausing()) {
                                mEglHelper.finish();
                                if (LOG_SURFACE) {
                                    Log.i("GLThread", "terminating EGL because paused tid=" + getId());
                                }
                            }
                        }

                        // Have we lost the SurfaceView surface?
                        if ((!view.mHasSurface) && (!view.mWaitingForSurface)) {
                            if (LOG_SURFACE) {
                                Log.i("GLThread", "noticed surfaceView surface lost tid=" + getId());
                            }
                            if (view.mHaveEglSurface) {
                                stopEglSurfaceLocked();
                            }
                            view.mWaitingForSurface = true;
                            view.mSurfaceIsBad = false;
                            view.sGLThreadManager.notifyAll();
                        }

                        // Have we acquired the surface view surface?
                        if (view.mHasSurface && view.mWaitingForSurface) {
                            if (LOG_SURFACE) {
                                Log.i("GLThread", "noticed surfaceView surface acquired tid=" + getId());
                            }
                            view.mWaitingForSurface = false;
                            view.sGLThreadManager.notifyAll();
                        }

                        if (doRenderNotification) {
                            if (LOG_SURFACE) {
                                Log.i("GLThread", "sending render notification tid=" + getId());
                            }
                            wantRenderNotification = false;
                            doRenderNotification = false;
                            view.mRenderComplete = true;
                            view.sGLThreadManager.notifyAll();
                        }

                        // Ready to draw?
                        if (view.readyToDraw()) {

                            // If we don't have an EGL context, try to acquire one.
                            if (!view.mHaveEglContext) {
                                if (askedToReleaseEglContext) {
                                    askedToReleaseEglContext = false;
                                } else if (view.sGLThreadManager.tryAcquireEglContextLocked(this)) {
                                    try {
                                        mEglHelper.start();
                                    } catch (RuntimeException t) {
                                        view.sGLThreadManager.releaseEglContextLocked(this);
                                        throw t;
                                    }
                                    view.mHaveEglContext = true;
                                    createEglContext = true;

                                    view.sGLThreadManager.notifyAll();
                                }
                            }

                            if (view.mHaveEglContext && !view.mHaveEglSurface) {
                            	view.mHaveEglSurface = true;
                                createEglSurface = true;
                                createGlInterface = true;
                                sizeChanged = true;
                            }

                            if (view.mHaveEglSurface) {
                                if (view.mSizeChanged) {
                                    sizeChanged = true;
                                    w = view.mWidth;
                                    h = view.mHeight;
                                    wantRenderNotification = true;
                                    if (LOG_SURFACE) {
                                        Log.i("GLThread",
                                                "noticing that we want render notification tid="
                                                + getId());
                                    }

                                    // Destroy and recreate the EGL surface.
                                    createEglSurface = true;

                                    view.mSizeChanged = false;
                                }
                                view.mRequestRender = false;
                                view.sGLThreadManager.notifyAll();
                                break;
                            }
                        }

                        // By design, this is the only place in a GLThread thread where we wait().
                        if (LOG_THREADS) {
                            Log.i("GLThread", "waiting tid=" + getId()
                                + " mHaveEglContext: " + view.mHaveEglContext
                                + " mHaveEglSurface: " + view.mHaveEglSurface
                                + " mFinishedCreatingEglSurface: " + view.mFinishedCreatingEglSurface
                                + " mPaused: " + view.mPaused
                                + " mHasSurface: " + view.mHasSurface
                                + " mSurfaceIsBad: " + view.mSurfaceIsBad
                                + " mWaitingForSurface: " + view.mWaitingForSurface
                                + " mWidth: " + view.mWidth
                                + " mHeight: " + view.mHeight
                                + " mRequestRender: " + view.mRequestRender
                                + " mRenderMode: " + mRenderMode);
                        }
                        view.sGLThreadManager.wait();
                    }
                } // end of synchronized(sGLThreadManager)

                if (event != null) {
                    event.run();
                    event = null;
                    continue;
                }

                if (createEglSurface) {
                    if (LOG_SURFACE) {
                        Log.w("GLThread", "egl createSurface");
                    }
                    if (mEglHelper.createSurface()) {
                        synchronized(view.sGLThreadManager) {
                        	view.mFinishedCreatingEglSurface = true;
                            view.sGLThreadManager.notifyAll();
                        }
                    } else {
                        synchronized(view.sGLThreadManager) {
                        	view.mFinishedCreatingEglSurface = true;
                            view.mSurfaceIsBad = true;
                            view.sGLThreadManager.notifyAll();
                        }
                        continue;
                    }
                    createEglSurface = false;
                }

                if (createGlInterface) {
                    gl = (GL10) mEglHelper.createGL();

                    view.sGLThreadManager.checkGLDriver(gl);
                    createGlInterface = false;
                }

                if (createEglContext) {
                    if (LOG_RENDERER) {
                        Log.w("GLThread", "onSurfaceCreated");
                    }
                    if (view != null) {
                        try {
                            //Trace.traceBegin(Trace.TRACE_TAG_VIEW, "onSurfaceCreated");
                            mRenderer.onSurfaceCreated(gl, mEglHelper.mEglConfig);
                        } finally {
                            //Trace.traceEnd(Trace.TRACE_TAG_VIEW);
                        }
                    }
                    createEglContext = false;
                }

                if (sizeChanged) {
                    if (LOG_RENDERER) {
                        Log.w("GLThread", "onSurfaceChanged(" + w + ", " + h + ")");
                    }
                    if (view != null) {
                        try {
                            //Trace.traceBegin(Trace.TRACE_TAG_VIEW, "onSurfaceChanged");
                            mRenderer.onSurfaceChanged(gl, w, h);
                        } finally {
                            //Trace.traceEnd(Trace.TRACE_TAG_VIEW);
                        }
                    }
                    sizeChanged = false;
                }

                if (LOG_RENDERER_DRAW_FRAME) {
                    Log.w("GLThread", "onDrawFrame tid=" + getId());
                }
                {
                    if (view != null) {
                        try {
                            //Trace.traceBegin(Trace.TRACE_TAG_VIEW, "onDrawFrame");
                            mRenderer.onDrawFrame(gl);
                        } finally {
                            //Trace.traceEnd(Trace.TRACE_TAG_VIEW);
                        }
                    }
                }
                int swapError = mEglHelper.swap();
                switch (swapError) {
                    case EGL10.EGL_SUCCESS:
                        break;
                    case EGL11.EGL_CONTEXT_LOST:
                        if (LOG_SURFACE) {
                            Log.i("GLThread", "egl context lost tid=" + getId());
                        }
                        lostEglContext = true;
                        break;
                    default:
                        // Other errors typically mean that the current surface is bad,
                        // probably because the SurfaceView surface has been destroyed,
                        // but we haven't been notified yet.
                        // Log the error to help developers understand why rendering stopped.
                        EglHelper.logEglErrorAsWarning("GLThread", "eglSwapBuffers", swapError);

                        synchronized(view.sGLThreadManager) {
                        	view.mSurfaceIsBad = true;
                        	view.sGLThreadManager.notifyAll();
                        }
                        break;
                }

                if (wantRenderNotification) {
                    doRenderNotification = true;
                }
            }
        } finally {
            /*
             * clean-up everything...
             */
            synchronized (mGLSurfaceViewWeakRef.get().sGLThreadManager) {
                stopEglSurfaceLocked();
                stopEglContextLocked();
            }
        }
    }

    public void setRenderMode(int renderMode) {
        if ( !((RENDERMODE_WHEN_DIRTY <= renderMode) && (renderMode <= RENDERMODE_CONTINUOUSLY)) ) {
            throw new IllegalArgumentException("renderMode");
        }
        synchronized(mGLSurfaceViewWeakRef.get().sGLThreadManager) {
            mRenderMode = renderMode;
            mGLSurfaceViewWeakRef.get().sGLThreadManager.notifyAll();
        }
    }

    public int getRenderMode() {
        synchronized(mGLSurfaceViewWeakRef.get().sGLThreadManager) {
            return mRenderMode;
        }
    }

    public void requestRender() {
        GameSurface view = mGLSurfaceViewWeakRef.get();
        synchronized(view.sGLThreadManager) {
        	view.mRequestRender = true;
        	view.sGLThreadManager.notifyAll();
        }
    }

    public void onPause() {
        GameSurface view = mGLSurfaceViewWeakRef.get();
        synchronized (view.sGLThreadManager) {
            if (LOG_PAUSE_RESUME) {
                Log.i("GLThread", "onPause tid=" + getId());
            }
            mRequestPaused = true;
            mGLSurfaceViewWeakRef.get().sGLThreadManager.notifyAll();
            while ((! view.mExited) && (! view.mPaused)) {
                if (LOG_PAUSE_RESUME) {
                    Log.i("Main thread", "onPause waiting for mPaused.");
                }
                try {
                	view.sGLThreadManager.wait();
                } catch (InterruptedException ex) {
                    Thread.currentThread().interrupt();
                }
            }
        }
    }

    public void onResume() {
        GameSurface view = mGLSurfaceViewWeakRef.get();
        synchronized (view.sGLThreadManager) {
            if (LOG_PAUSE_RESUME) {
                Log.i("GLThread", "onResume tid=" + getId());
            }
            mRequestPaused = false;
            view.mRequestRender = true;
            view.mRenderComplete = false;
            view.sGLThreadManager.notifyAll();
            while ((!view.mExited) && view.mPaused && (!view.mRenderComplete)) {
                if (LOG_PAUSE_RESUME) {
                    Log.i("Main thread", "onResume waiting for !mPaused.");
                }
                try {
                	view.sGLThreadManager.wait();
                } catch (InterruptedException ex) {
                    Thread.currentThread().interrupt();
                }
            }
        }
    }

    public void requestExitAndWait() {
        // don't call this from GLThread thread or it is a guaranteed
        // deadlock!
        GameSurface view = mGLSurfaceViewWeakRef.get();
        synchronized(view.sGLThreadManager) {
            mShouldExit = true;
            view.sGLThreadManager.notifyAll();
            while (!view.mExited) 
            {
                try {
                	view.sGLThreadManager.wait();
                } catch (InterruptedException ex) {
                    Thread.currentThread().interrupt();
                }
            }
        }
    }

    public void requestReleaseEglContextLocked() {
        mShouldReleaseEglContext = true;
        mGLSurfaceViewWeakRef.get().sGLThreadManager.notifyAll();
    }

    /**
     * Queue an "event" to be run on the GL rendering thread.
     * @param r the runnable to be run on the GL rendering thread.
     */
    public void queueEvent(Runnable r) {
        if (r == null) {
            throw new IllegalArgumentException("r must not be null");
        }
        synchronized(mGLSurfaceViewWeakRef.get().sGLThreadManager) {
            mEventQueue.add(r);
            mGLSurfaceViewWeakRef.get().sGLThreadManager.notifyAll();
        }
    }

    // Once the thread is started, all accesses to the following member
    // variables are protected by the sGLThreadManager monitor
    private boolean mShouldExit;
    private boolean mRequestPaused;
    private boolean mShouldReleaseEglContext;
    private int mRenderMode;
	private Renderer mRenderer;
    private ArrayList<Runnable> mEventQueue = new ArrayList<Runnable>();

    // End of member variables protected by the sGLThreadManager monitor.

    private EglHelper mEglHelper;

    /**
     * Set once at thread construction time, nulled out when the parent view is garbage
     * called. This weak reference allows the GLSurfaceView to be garbage collected while
     * the GLThread is still alive.
     */
    private WeakReference<GameSurface> mGLSurfaceViewWeakRef;

}
