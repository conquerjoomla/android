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
class GLThread extends Thread 
{
    /**
     * The renderer only renders
     * when the surface is created, or when {@link #requestRender} is called.
     *
     * @see #getRenderMode()
     * @see #setRenderMode(int)
     */
    public final static int RENDERMODE_WHEN_DIRTY = 0;
    /**
     * The renderer is called
     * continuously to re-render the scene.
     *
     * @see #getRenderMode()
     * @see #setRenderMode(int)
     * @see #requestRender()
     */
    public final static int RENDERMODE_CONTINUOUSLY = 1;
    /**
     * A generic renderer interface.
     * <p>
     * The renderer is responsible for making OpenGL calls to render a frame.
     * <p>
     * GLSurfaceView2 clients typically create their own classes that implement
     * this interface, and then call {@link GLSurfaceView2#setRenderer} to
     * register the renderer with the GLSurfaceView2.
     * <p>
     * <h3>Threading</h3>
     * The renderer will be called on a separate thread, so that rendering
     * performance is decoupled from the UI thread. Clients typically need to
     * communicate with the renderer from the UI thread, because that's where
     * input events are received. Clients can communicate using any of the
     * standard Java techniques for cross-thread communication, or they can
     * use the {@link GLSurfaceView2#queueEvent(Runnable)} convenience method.
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
         * starts and whenever the EGL context is lost. The context will typically
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

    private class EglHelper {
        public EglHelper(GameSurface surface)
        {
        	mSurface = surface;
        }

        /**
         * Initialize EGL for a given configuration spec.
         * @param configSpec
         */
        public void start(){
            /*
             * Get an EGL instance
             */
            mEgl = (EGL10) EGLContext.getEGL();

            /*
             * Get to the default display.
             */
            mEglDisplay = mEgl.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY);

            /*
             * We can now initialize EGL for that display
             */
            int[] version = new int[2];
            mEgl.eglInitialize(mEglDisplay, version);
            mEglConfig = mSurface.mEGLConfigChooser.chooseConfig(mEgl, mEglDisplay);

            /*
            * Create an OpenGL ES context. This must be done only once, an
            * OpenGL context is a somewhat heavy object.
            */
            mEglContext = mSurface.mEGLContextFactory.createContext(mEgl, mEglDisplay, mEglConfig);

            mEglSurface = null;
        }

        /*
         * React to the creation of a new surface by creating and returning an
         * OpenGL interface that renders to that surface.
         */
        public GL createSurface()
        {
            /*
             *  The window size has changed, so we need to create a new
             *  surface.
             */
            if (mEglSurface != null) {

                /*
                 * Unbind and destroy the old EGL surface, if
                 * there is one.
                 */
                mEgl.eglMakeCurrent(mEglDisplay, EGL10.EGL_NO_SURFACE,
                        EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_CONTEXT);
                mSurface.mEGLWindowSurfaceFactory.destroySurface(mEgl, mEglDisplay, mEglSurface);
            }

            /*
             * Create an EGL surface we can render into.
             */
            mEglSurface = mSurface.mEGLWindowSurfaceFactory.createWindowSurface(mEgl,
                    mEglDisplay, mEglConfig, mSurface.getHolder());

            /*
             * Before we can issue GL commands, we need to make sure
             * the context is current and bound to a surface.
             */
            mEgl.eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface,
                    mEglContext);

            GL gl = mEglContext.getGL();
            return gl;
        }

        /**
         * Display the current render surface.
         * @return false if the context has been lost.
         */
        public boolean swap() {
            mEgl.eglSwapBuffers(mEglDisplay, mEglSurface);

            /*
             * Always check for EGL_CONTEXT_LOST, which means the context
             * and all associated data were lost (For instance because
             * the device went to sleep). We need to sleep until we
             * get a new surface.
             */
            return mEgl.eglGetError() != EGL11.EGL_CONTEXT_LOST;
        }

        public void destroySurface() {
            if (mEglSurface != null) {
                mEgl.eglMakeCurrent(mEglDisplay, EGL10.EGL_NO_SURFACE,
                        EGL10.EGL_NO_SURFACE,
                        EGL10.EGL_NO_CONTEXT);
                mSurface.mEGLWindowSurfaceFactory.destroySurface(mEgl, mEglDisplay, mEglSurface);
                mEglSurface = null;
            }
        }

        public void finish() {
            if (mEglContext != null) {
                mSurface.mEGLContextFactory.destroyContext(mEgl, mEglDisplay, mEglContext);
                mEglContext = null;
            }
            if (mEglDisplay != null) {
                mEgl.eglTerminate(mEglDisplay);
                mEglDisplay = null;
            }
        }

        GameSurface mSurface;
        EGL10 mEgl;
        EGLDisplay mEglDisplay;
        EGLSurface mEglSurface;
        EGLConfig mEglConfig;
        EGLContext mEglContext;
    }

    GLThread(GameSurface Surface, Renderer renderer )
    {
		super();
		mDone = false;
		mWidth = 0;
		mHeight = 0;
		mRequestRender = true;
		mRenderMode = RENDERMODE_CONTINUOUSLY;
        mSurface = Surface;
		mRenderer = renderer;
		setName("GLThread");
	}

	@Override
	public void run() {
		/*
		 * When the android framework launches a second instance of
		 * an activity, the new instance's onCreate() method may be
		 * called before the first instance returns from onDestroy().
		 *
		 * This semaphore ensures that only one instance at a time
		 * accesses EGL.
		 */
		try {
			try {
				mSurface.sEglSemaphore.acquire();
			} catch (InterruptedException e) {
				return;
			}
			guardedRun();
		} catch (InterruptedException e) {
			// fall thru and exit normally
		} finally {
			mSurface.sEglSemaphore.release();
		}
	}

	private void guardedRun() throws InterruptedException {
		mEglHelper = new EglHelper(mSurface);
		try {
			mEglHelper.start();

			GL10 gl = null;
			boolean tellRendererSurfaceCreated = true;
			boolean tellRendererSurfaceChanged = true;

			/*
			 * This is our main activity thread's loop, we go until
			 * asked to quit.
			 */
			while (!mDone) {

				/*
				 *  Update the asynchronous state (window size)
				 */
				int w, h;
				boolean changed;
				boolean needStart = false;
				synchronized (this) {
					Runnable r;
					while ((r = getEvent()) != null) {
						r.run();
					}
					if (mPaused) {
						mEglHelper.destroySurface();
						mEglHelper.finish();
						needStart = true;
					}
					while (needToWait()) {
						if (!mHasSurface) {
							if (!mWaitingForSurface) {
								mEglHelper.destroySurface();
								mWaitingForSurface = true;
								notify();
							}
						}
						wait();
					}
					if (mDone) {
						break;
					}
					changed = mSizeChanged;
					w = mWidth;
					h = mHeight;
					mSizeChanged = false;
					mRequestRender = false;
					if (mHasSurface && mWaitingForSurface) {
						changed = true;
						mWaitingForSurface = false;
					}
				}
				if (needStart) {
					mEglHelper.start();
					tellRendererSurfaceCreated = true;
					changed = true;
				}
				if (changed) {
					gl = (GL10) mEglHelper.createSurface();
					tellRendererSurfaceChanged = true;
				}
				if (tellRendererSurfaceCreated) {
					mRenderer.onSurfaceCreated(gl, mEglHelper.mEglConfig);
					tellRendererSurfaceCreated = false;
				}
				if (tellRendererSurfaceChanged) {
					mRenderer.onSurfaceChanged(gl, w, h);
					tellRendererSurfaceChanged = false;
				}
				if ((w > 0) && (h > 0)) {
					/* draw a frame here */
					mRenderer.onDrawFrame(gl);

					/*
					 * Once we're done with GL, we need to call swapBuffers()
					 * to instruct the system to display the rendered frame
					 */
					mEglHelper.swap();
				}
			 }
		} finally {
			/*
			 * clean-up everything...
			 */
			mEglHelper.destroySurface();
			mEglHelper.finish();
		}
	}

	private boolean needToWait() {
		if (mDone) {
			return false;
		}

		if (mPaused || (! mHasSurface)) {
			return true;
		}

		if ((mWidth > 0) && (mHeight > 0) && (mRequestRender || (mRenderMode == RENDERMODE_CONTINUOUSLY))) {
			return false;
		}

		return true;
	}

	public void setRenderMode(int renderMode) {
		if ( !((RENDERMODE_WHEN_DIRTY <= renderMode) && (renderMode <= RENDERMODE_CONTINUOUSLY)) ) {
			throw new IllegalArgumentException("renderMode");
		}
		synchronized(this) {
			mRenderMode = renderMode;
			if (renderMode == RENDERMODE_CONTINUOUSLY) {
				notify();
			}
		}
	}

	public int getRenderMode() 
	{
		synchronized(this) {
			return mRenderMode;
		}
	}

	public void requestRender() {
		synchronized(this) {
			mRequestRender = true;
			notify();
		}
	}

	public void surfaceCreated() 
	{
		synchronized(this) {
			mHasSurface = true;
			notify();
		}
	}

	public void surfaceDestroyed() 
	{
		synchronized(this) {
			mHasSurface = false;
			notify();
			while(!mWaitingForSurface && isAlive()) {
				try {
					wait();
				} catch (InterruptedException e) {
					Thread.currentThread().interrupt();
				}
			}
		}
	}

	public void onPause() {
		synchronized (this) {
			mPaused = true;
		}
	}

	public void onResume() {
		synchronized (this) {
			mPaused = false;
			notify();
		}
	}

	public void onWindowResize(int w, int h) {
		synchronized (this) {
			mWidth = w;
			mHeight = h;
			mSizeChanged = true;
			notify();
		}
	}

	public void requestExitAndWait() {
		// don't call this from GLThread thread or it is a guaranteed
		// deadlock!
		synchronized(this) {
			mDone = true;
			notify();
		}
		try {
			join();
		} catch (InterruptedException ex) {
			Thread.currentThread().interrupt();
		}
	}

	/**
	 * Queue an "event" to be run on the GL rendering thread.
	 * @param r the runnable to be run on the GL rendering thread.
	 */
	public void queueEvent(Runnable r) {
		synchronized(this) {
			mEventQueue.add(r);
		}
	}

	private Runnable getEvent() {
		synchronized(this) {
			if (mEventQueue.size() > 0) {
				return mEventQueue.remove(0);
			}

		}
		return null;
	}

    private GameSurface mSurface;
	private boolean mDone;
	private boolean mPaused;
	private boolean mHasSurface;
	public boolean mWaitingForSurface;
	private int mWidth;
	private int mHeight;
	private int mRenderMode;
	private boolean mRequestRender;
	private Renderer mRenderer;
	private ArrayList<Runnable> mEventQueue = new ArrayList<Runnable>();
    private boolean mSizeChanged = true;
	private EglHelper mEglHelper;
}
