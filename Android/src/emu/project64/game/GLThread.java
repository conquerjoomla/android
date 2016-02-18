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
class GLThread
{
    private final static boolean LOG_THREADS = false;
    private final static boolean LOG_SURFACE = false;
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

    public interface SurfaceInfo {
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
         * @param mGl the GL interface. Use <code>instanceof</code> to
         * test if the interface supports GL11 or higher interfaces.
         * @param config the EGLConfig of the created surface. Can be used
         * to create matching pbuffers.
         */
        void onSurfaceCreated(GL10 mGl, EGLConfig config);

        /**
         * Called when the surface changed size.
         * <p>
         * Called after the surface is created and whenever
         * the OpenGL ES surface size changes.
         * <p>
         * Typically you will set your viewport here. If your camera
         * is fixed then you could also set your projection matrix here:
         * <pre class="prettyprint">
         * void onSurfaceChanged(GL10 mGl, int width, int height) {
         *     mGl.glViewport(0, 0, width, height);
         *     // for a fixed camera, set the projection too
         *     float ratio = (float) width / height;
         *     mGl.glMatrixMode(GL10.GL_PROJECTION);
         *     mGl.glLoadIdentity();
         *     mGl.glFrustumf(-ratio, ratio, -1, 1, 1, 10);
         * }
         * </pre>
         * @param mGl the GL interface. Use <code>instanceof</code> to
         * test if the interface supports GL11 or higher interfaces.
         * @param width
         * @param height
         */
        void onSurfaceChanged(GL10 mGl, int width, int height);
    }

    GLThread(WeakReference<GameSurface> glSurfaceViewWeakRef, SurfaceInfo surfaceInfo)
    {
        super();
        glSurfaceViewWeakRef.get().mRequestRender = true;
        mGLSurfaceViewWeakRef = glSurfaceViewWeakRef;
        mSurfaceInfo = surfaceInfo;
    }

    public void ThreadStarting() 
    {
        if (LOG_THREADS) {
            Log.i("GLThread", "starting tid=" + Thread.currentThread().getId());
        }
        GameSurface view = mGLSurfaceViewWeakRef.get();
        view.mHaveEglContext = false;
        view.mHaveEglSurface = false;
    }

    public void ThreadExiting()
    {
        /*
         * clean-up everything...
         */
        GameSurface view = mGLSurfaceViewWeakRef.get();
        view.sGLThreadManager.threadExiting(this);
		synchronized (view.sGLThreadManager) {
			stopEglSurfaceLocked();
			stopEglContextLocked();
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
            view.mEglHelper.destroySurface();
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
            view.mEglHelper.finish();
            view.mHaveEglContext = false;
            mGLSurfaceViewWeakRef.get().sGLThreadManager.releaseEglContextLocked(this);
        }
    }

    public void ReadyToDraw() 
    {
        GameSurface view = mGLSurfaceViewWeakRef.get();
        try {
			synchronized (view.sGLThreadManager) {
				while (true) {
					if (mShouldExit) {
						return;
					}

					if (! mEventQueue.isEmpty()) {
						mEvent = mEventQueue.remove(0);
						break;
					}

					// Update the pause state.
					boolean pausing = false;
					if (view.mPaused != mRequestPaused) {
						pausing = mRequestPaused;
						view.mPaused = mRequestPaused;
						view.sGLThreadManager.notifyAll();
						if (LOG_PAUSE_RESUME) {
							Log.i("GLThread", "mPaused is now " + view.mPaused + " tid=" + Thread.currentThread().getId());
						}
					}

					// Do we need to give up the EGL context?
					if (mShouldReleaseEglContext) {
						if (LOG_SURFACE) {
							Log.i("GLThread", "releasing EGL context because asked to tid=" + Thread.currentThread().getId());
						}
						stopEglSurfaceLocked();
						stopEglContextLocked();
						mShouldReleaseEglContext = false;
						mAskedToReleaseEglContext = true;
					}

					// Have we lost the EGL context?
					if (mLostEglContext) {
						stopEglSurfaceLocked();
						stopEglContextLocked();
						mLostEglContext = false;
					}

					// When pausing, release the EGL surface:
					if (pausing && view.mHaveEglSurface) {
						if (LOG_SURFACE) {
							Log.i("GLThread", "releasing EGL surface because paused tid=" + Thread.currentThread().getId());
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
								Log.i("GLThread", "releasing EGL context because paused tid=" + Thread.currentThread().getId());
							}
						}
					}

					// When pausing, optionally terminate EGL:
					if (pausing) {
						if (view.sGLThreadManager.shouldTerminateEGLWhenPausing()) {
							view.mEglHelper.finish();
							if (LOG_SURFACE) {
								Log.i("GLThread", "terminating EGL because paused tid=" + Thread.currentThread().getId());
							}
						}
					}

					// Have we lost the SurfaceView surface?
					if ((!view.mHasSurface) && (!view.mWaitingForSurface)) {
						if (LOG_SURFACE) {
							Log.i("GLThread", "noticed surfaceView surface lost tid=" + Thread.currentThread().getId());
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
							Log.i("GLThread", "noticed surfaceView surface acquired tid=" + Thread.currentThread().getId());
						}
						view.mWaitingForSurface = false;
						view.sGLThreadManager.notifyAll();
					}

					if (mDoRenderNotification) {
						if (LOG_SURFACE) {
							Log.i("GLThread", "sending render notification tid=" + Thread.currentThread().getId());
						}
						mWantRenderNotification = false;
						mDoRenderNotification = false;
						view.mRenderComplete = true;
						view.sGLThreadManager.notifyAll();
					}

					// Ready to draw?
					if (view.readyToDraw()) {

						// If we don't have an EGL context, try to acquire one.
						if (!view.mHaveEglContext) {
							if (mAskedToReleaseEglContext) {
								mAskedToReleaseEglContext = false;
							} else if (view.sGLThreadManager.tryAcquireEglContextLocked(this)) {
								try {
									view.mEglHelper.start();
								} catch (RuntimeException t) {
									view.sGLThreadManager.releaseEglContextLocked(this);
									throw t;
								}
								view.mHaveEglContext = true;
								mCreateEglContext = true;

								view.sGLThreadManager.notifyAll();
							}
						}

						if (view.mHaveEglContext && !view.mHaveEglSurface) {
							view.mHaveEglSurface = true;
							mCreateEglSurface = true;
							mCreateGlInterface = true;
							mSizeChanged = true;
						}

						if (view.mHaveEglSurface) {
							if (view.mSizeChanged) {
								mSizeChanged = true;
								mW = view.mWidth;
								mH = view.mHeight;
								mWantRenderNotification = true;
								if (LOG_SURFACE) {
									Log.i("GLThread",
											"noticing that we want render notification tid="
											+ Thread.currentThread().getId());
								}

								// Destroy and recreate the EGL surface.
								mCreateEglSurface = true;

								view.mSizeChanged = false;
							}
							view.mRequestRender = false;
							view.sGLThreadManager.notifyAll();
							break;
						}
					}

					// By design, this is the only place in a GLThread thread where we wait().
					if (LOG_THREADS) {
						Log.i("GLThread", "waiting tid=" + Thread.currentThread().getId()
							+ " mHaveEglContext: " + view.mHaveEglContext
							+ " mHaveEglSurface: " + view.mHaveEglSurface
							+ " mFinishedCreatingEglSurface: " + view.mFinishedCreatingEglSurface
							+ " mPaused: " + view.mPaused
							+ " mHasSurface: " + view.mHasSurface
							+ " mSurfaceIsBad: " + view.mSurfaceIsBad
							+ " mWaitingForSurface: " + view.mWaitingForSurface
							+ " mWidth: " + view.mWidth
							+ " mHeight: " + view.mHeight
							+ " mRequestRender: " + view.mRequestRender);
					}

					try {
						view.sGLThreadManager.wait();
			        } catch (InterruptedException e) {
			            // fall thru and exit normally
			        }
				}
			} // end of synchronized(sGLThreadManager)

			if (mEvent != null) {
				mEvent.run();
				mEvent = null;
				ReadyToDraw();
				return;
			}

			if (mCreateEglSurface) {
				if (LOG_SURFACE) {
					Log.w("GLThread", "egl createSurface");
				}
				if (view.mEglHelper.createSurface()) {
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
					ReadyToDraw();
					return;
				}
				mCreateEglSurface = false;
			}

			if (mCreateGlInterface) {
				mGl = (GL10) view.mEglHelper.createGL();

				view.sGLThreadManager.checkGLDriver(mGl);
				mCreateGlInterface = false;
			}

			if (mCreateEglContext) {
				if (LOG_RENDERER) {
					Log.w("GLThread", "onSurfaceCreated");
				}
				if (view != null) {
					try {
						//Trace.traceBegin(Trace.TRACE_TAG_VIEW, "onSurfaceCreated");
						mSurfaceInfo.onSurfaceCreated(mGl, view.mEglHelper.mEglConfig);
					} finally {
						//Trace.traceEnd(Trace.TRACE_TAG_VIEW);
					}
				}
				mCreateEglContext = false;
			}

			if (mSizeChanged) {
				if (LOG_RENDERER) {
					Log.w("GLThread", "onSurfaceChanged(" + mW + ", " + mH + ")");
				}
				if (view != null) {
					try {
						//Trace.traceBegin(Trace.TRACE_TAG_VIEW, "onSurfaceChanged");
						mSurfaceInfo.onSurfaceChanged(mGl, mW, mH);
					} finally {
						//Trace.traceEnd(Trace.TRACE_TAG_VIEW);
					}
				}
				mSizeChanged = false;
			}
        } finally {
            /*
             * clean-up everything...
             */
        }
    }

    public void SwapBuffers()
    {
		Log.i("GLThread", "SwapBuffers()");

        GameSurface view = mGLSurfaceViewWeakRef.get();
		int swapError = view.mEglHelper.swap();
		switch (swapError) {
			case EGL10.EGL_SUCCESS:
				break;
			case EGL11.EGL_CONTEXT_LOST:
				if (LOG_SURFACE) {
					Log.i("GLThread", "egl context lost tid=" + Thread.currentThread().getId());
				}
				mLostEglContext = true;
				break;
			default:
				// Other errors typically mean that the current surface is bad,
				// probably because the SurfaceView surface has been destroyed,
				// but we haven't been notified yet.
				// Log the error to help developers understand why rendering stopped.
				view.mEglHelper.logEglErrorAsWarning("GLThread", "eglSwapBuffers", swapError);

				synchronized(view.sGLThreadManager) {
					view.mSurfaceIsBad = true;
					view.sGLThreadManager.notifyAll();
				}
				break;
		}

		if (mWantRenderNotification) {
			mDoRenderNotification = true;
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
                Log.i("GLThread", "onPause tid=" + Thread.currentThread().getId());
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
                Log.i("GLThread", "onResume tid=" + Thread.currentThread().getId());
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
     * Queue an "mEvent" to be run on the GL rendering thread.
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
    private SurfaceInfo mSurfaceInfo;
    private ArrayList<Runnable> mEventQueue = new ArrayList<Runnable>();

    // End of member variables protected by the sGLThreadManager monitor.

    /**
     * Set once at thread construction time, nulled out when the parent view is garbage
     * called. This weak reference allows the GLSurfaceView to be garbage collected while
     * the GLThread is still alive.
     */
    private WeakReference<GameSurface> mGLSurfaceViewWeakRef;

	GL10 mGl = null;
	boolean mCreateEglContext = false;
	boolean mCreateEglSurface = false;
	boolean mCreateGlInterface = false;
	boolean mLostEglContext = false;
	boolean mSizeChanged = false;
	boolean mWantRenderNotification = false;
	boolean mDoRenderNotification = false;
	boolean mAskedToReleaseEglContext = false;
	int mW = 0;
	int mH = 0;
	Runnable mEvent = null;
}
