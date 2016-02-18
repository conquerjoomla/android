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

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.os.Build;
import android.os.Trace;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class GameSurface extends SurfaceView implements SurfaceHolder.Callback
{
	private boolean rendererSet;

	private final static boolean LOG_THREADS = false;
    private final static boolean LOG_SURFACE = false;

	// LogCat strings for debugging, defined here to simplify maintenance/lookup
    private static final String TAG = "GameSurface";

    /**
     * Constructor that is called when inflating a view from XML. This is called when a view is
     * being constructed from an XML file, supplying attributes that were specified in the XML file.
     * This version uses a default style of 0, so the only attribute values applied are those in the
     * Context's Theme and the given AttributeSet. The method onFinishInflate() will be called after
     * all children have been added.
     * 
     * @param context The Context the view is running in, through which it can access the current
     *            theme, resources, etc.
     * @param attrs The attributes of the XML tag that is inflating the view.
     */
    public GameSurface( Context context, AttributeSet attribs )
    {
        super( context, attribs );
        init();
    }

    @Override
    protected void finalize() throws Throwable 
    {
        super.finalize();
    }

    private void init() {
        // Install a SurfaceHolder.Callback so we get notified when the
        // underlying surface is created and destroyed
        SurfaceHolder holder = getHolder();
        holder.addCallback(this);
        // setFormat is done by SurfaceView in SDK 2.3 and newer. Uncomment
        // this statement if back-porting to 2.2 or older:
        // holder.setFormat(PixelFormat.RGB_565);
        //
        // setType is not needed for SDK 2.0 or newer. Uncomment this
        // statement if back-porting this code to older SDKs.
        // holder.setType(SurfaceHolder.SURFACE_TYPE_GPU);
        if (mEGLConfigChooser == null) {
            mEGLConfigChooser = new SimpleEGLConfigChooser(true);
        }
        if (mEGLContextFactory == null) {
            mEGLContextFactory = new DefaultContextFactory();
        }
        if (mEGLWindowSurfaceFactory == null) {
            mEGLWindowSurfaceFactory = new DefaultWindowSurfaceFactory();
        }
    }
    
    public boolean createGLContext( ActivityManager activityManager )
    {
		ConfigurationInfo configurationInfo = activityManager.getDeviceConfigurationInfo();

		final boolean supportsEs2 = configurationInfo.reqGlEsVersion >= 0x20000 || isProbablyEmulator();
		if (supportsEs2) 
		{
			if (isProbablyEmulator()) 
			{
				// Avoids crashes on startup with some emulator images.
				this.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
			}

			this.setEGLContextClientVersion(2);
			rendererSet = true;
		}
		else 
		{
			// Should never be seen in production, since the manifest filters
			// unsupported devices.
			Log.e(TAG, "This device does not support OpenGL ES 2.0.");
	        return false;
		}
        return true;
    }
	
    private boolean isProbablyEmulator() 
	{
		return Build.VERSION.SDK_INT >= Build.VERSION_CODES.ICE_CREAM_SANDWICH_MR1
				&& (Build.FINGERPRINT.startsWith("generic")
				|| Build.FINGERPRINT.startsWith("unknown")
				|| Build.MODEL.contains("google_sdk")
				|| Build.MODEL.contains("Emulator")
				|| Build.MODEL.contains("Android SDK built for x86"));
	}
    
    public boolean ableToDraw() {
        return mHaveEglContext && mHaveEglSurface && readyToDraw();
    }

    boolean readyToDraw() {
        return (!mPaused) && mHasSurface && (!mSurfaceIsBad)
            && (mWidth > 0) && (mHeight > 0);
    }

    /**
     * Install a custom EGLWindowSurfaceFactory.
     * <p>If this method is
     * called, it must be called before {@link #setRenderer(Renderer)}
     * is called.
     * <p>
     * If this method is not called, then by default
     * a window surface will be created with a null attribute list.
     */
    public void setEGLWindowSurfaceFactory(EGLWindowSurfaceFactory factory) {
        mEGLWindowSurfaceFactory = factory;
    }

    /**
     * Install a custom EGLConfigChooser.
     * <p>If this method is
     * called, it must be called before {@link #setRenderer(Renderer)}
     * is called.
     * <p>
     * If no setEGLConfigChooser method is called, then by default the
     * view will choose an EGLConfig that is compatible with the current
     * android.view.Surface, with a depth buffer depth of
     * at least 16 bits.
     * @param configChooser
     */
    public void setEGLConfigChooser(EGLConfigChooser configChooser) {
        mEGLConfigChooser = configChooser;
    }

    /**
     * Install a config chooser which will choose a config
     * with at least the specified depthSize and stencilSize,
     * and exactly the specified redSize, greenSize, blueSize and alphaSize.
     * <p>If this method is
     * called, it must be called before {@link #setRenderer(Renderer)}
     * is called.
     * <p>
     * If no setEGLConfigChooser method is called, then by default the
     * view will choose an RGB_888 surface with a depth buffer depth of
     * at least 16 bits.
     *
     */
    public void setEGLConfigChooser(int redSize, int greenSize, int blueSize,
            int alphaSize, int depthSize, int stencilSize) {
        setEGLConfigChooser(new ComponentSizeChooser(redSize, greenSize,
                blueSize, alphaSize, depthSize, stencilSize));
    }

    /**
     * Inform the default EGLContextFactory and default EGLConfigChooser
     * which EGLContext client version to pick.
     * <p>Use this method to create an OpenGL ES 2.0-compatible context.
     * Example:
     * <pre class="prettyprint">
     *     public MyView(Context context) {
     *         super(context);
     *         setEGLContextClientVersion(2); // Pick an OpenGL ES 2.0 context.
     *         setRenderer(new MyRenderer());
     *     }
     * </pre>
     * <p>Note: Activities which require OpenGL ES 2.0 should indicate this by
     * setting @lt;uses-feature android:glEsVersion="0x00020000" /> in the activity's
     * AndroidManifest.xml file.
     * <p>If this method is called, it must be called before {@link #setRenderer(Renderer)}
     * is called.
     * <p>This method only affects the behavior of the default EGLContexFactory and the
     * default EGLConfigChooser. If
     * {@link #setEGLContextFactory(EGLContextFactory)} has been called, then the supplied
     * EGLContextFactory is responsible for creating an OpenGL ES 2.0-compatible context.
     * If
     * {@link #setEGLConfigChooser(EGLConfigChooser)} has been called, then the supplied
     * EGLConfigChooser is responsible for choosing an OpenGL ES 2.0-compatible config.
     * @param version The EGLContext client version to choose. Use 2 for OpenGL ES 2.0
     */
    public void setEGLContextClientVersion(int version) {
        mEGLContextClientVersion = version;
    }
    /**
     * This method is part of the SurfaceHolder.Callback interface, and is
     * not normally called or subclassed by clients of GLSurfaceView.
     */
    public void surfaceCreated(SurfaceHolder holder) 
    {
        synchronized(sGLThreadManager) {
            if (LOG_THREADS) {
                Log.i("GLThread", "surfaceCreated tid=" + getId());
            }
            mHasSurface = true;
            mFinishedCreatingEglSurface = false;
            sGLThreadManager.notifyAll();
            while (mWaitingForSurface
                   && !mFinishedCreatingEglSurface
                   && !mExited) {
                try {
                    sGLThreadManager.wait();
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                }
            }
        }
    }

    /**
     * This method is part of the SurfaceHolder.Callback interface, and is
     * not normally called or subclassed by clients of GLSurfaceView.
     */
    public void surfaceDestroyed(SurfaceHolder holder) {
        synchronized(sGLThreadManager) {
            if (LOG_THREADS) {
                Log.i("GLThread", "surfaceDestroyed tid=" + getId());
            }
            mHasSurface = false;
            sGLThreadManager.notifyAll();
            while((!mWaitingForSurface) && (!mExited)) {
                try {
                    sGLThreadManager.wait();
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                }
            }
        }
    }

    /**
     * This method is part of the SurfaceHolder.Callback interface, and is
     * not normally called or subclassed by clients of GLSurfaceView.
     */
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) 
    {
        synchronized (sGLThreadManager) {
            mWidth = w;
            mHeight = h;
            mSizeChanged = true;
            mRequestRender = true;
            mRenderComplete = false;
            sGLThreadManager.notifyAll();

            // Wait for thread to react to resize and render a frame
            while (! mExited && !mPaused && !mRenderComplete
                    && ableToDraw()) {
                if (LOG_SURFACE) {
                    Log.i("Main thread", "onWindowResize waiting for render complete from tid=" + getId());
                }
                try {
                    sGLThreadManager.wait();
                } catch (InterruptedException ex) {
                    Thread.currentThread().interrupt();
                }
            }
        }
    }

    /**
     * An interface used to wrap a GL interface.
     * <p>Typically
     * used for implementing debugging and tracing on top of the default
     * GL interface. You would typically use this by creating your own class
     * that implemented all the GL methods by delegating to another GL instance.
     * Then you could add your own behavior before or after calling the
     * delegate. All the GLWrapper would do was instantiate and return the
     * wrapper GL instance:
     * <pre class="prettyprint">
     * class MyGLWrapper implements GLWrapper {
     *     GL wrap(GL gl) {
     *         return new MyGLImplementation(gl);
     *     }
     *     static class MyGLImplementation implements GL,GL10,GL11,... {
     *         ...
     *     }
     * }
     * </pre>
     * @see #setGLWrapper(GLWrapper)
     */
    public interface GLWrapper {
        /**
         * Wraps a gl interface in another gl interface.
         * @param gl a GL interface that is to be wrapped.
         * @return either the input argument or another GL object that wraps the input argument.
         */
        GL wrap(GL gl);
    }

    /**
     * An interface for customizing the eglCreateContext and eglDestroyContext calls.
     * <p>
     * This interface must be implemented by clients wishing to call
     * {@link GLSurfaceView#setEGLContextFactory(EGLContextFactory)}
     */
    public interface EGLContextFactory {
        EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig eglConfig);
        void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context);
    }

    private class DefaultContextFactory implements EGLContextFactory {
        private int EGL_CONTEXT_CLIENT_VERSION = 0x3098;

        public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig config)
        {
            int[] attrib_list = {EGL_CONTEXT_CLIENT_VERSION, mEGLContextClientVersion,
                    EGL10.EGL_NONE };

            return egl.eglCreateContext(display, config, EGL10.EGL_NO_CONTEXT,
            		mEGLContextClientVersion != 0 ? attrib_list : null);
        }

        public void destroyContext(EGL10 egl, EGLDisplay display,
                EGLContext context) {
            if (!egl.eglDestroyContext(display, context)) {
                Log.e("DefaultContextFactory", "display:" + display + " context: " + context);
                if (LOG_THREADS) {
                    Log.i("DefaultContextFactory", "tid=" + Thread.currentThread().getId());
                }
                String message = "eglDestroyContex failed: " + EGLLogWrapper.getErrorString(egl.eglGetError());
                if (LOG_THREADS) {
                    Log.e("EglHelper", "throwEglException tid=" + Thread.currentThread().getId() + " "
                            + message);
                }
                throw new RuntimeException(message);

            }
        }
    }
	
    /**
     * An interface for customizing the eglCreateWindowSurface and eglDestroySurface calls.
     * <p>
     * This interface must be implemented by clients wishing to call
     * {@link GLSurfaceView#setEGLWindowSurfaceFactory(EGLWindowSurfaceFactory)}
     */
    public interface EGLWindowSurfaceFactory {
        /**
         *  @return null if the surface cannot be constructed.
         */
        EGLSurface createWindowSurface(EGL10 egl, EGLDisplay display, EGLConfig config,
                Object nativeWindow);
        void destroySurface(EGL10 egl, EGLDisplay display, EGLSurface surface);
    }

    private static class DefaultWindowSurfaceFactory implements EGLWindowSurfaceFactory {

        public EGLSurface createWindowSurface(EGL10 egl, EGLDisplay display,
                EGLConfig config, Object nativeWindow) {
            EGLSurface result = null;
            try {
                result = egl.eglCreateWindowSurface(display, config, nativeWindow, null);
            } catch (IllegalArgumentException e) {
                // This exception indicates that the surface flinger surface
                // is not valid. This can happen if the surface flinger surface has
                // been torn down, but the application has not yet been
                // notified via SurfaceHolder.Callback.surfaceDestroyed.
                // In theory the application should be notified first,
                // but in practice sometimes it is not. See b/4588890
                Log.e(TAG, "eglCreateWindowSurface", e);
            }
            return result;
        }

        public void destroySurface(EGL10 egl, EGLDisplay display,
                EGLSurface surface) {
            egl.eglDestroySurface(display, surface);
        }
    }

    /**
     * An interface for choosing an EGLConfig configuration from a list of
     * potential configurations.
     * <p>
     * This interface must be implemented by clients wishing to call
     * {@link GLSurfaceView#setEGLConfigChooser(EGLConfigChooser)}
     */
    public interface EGLConfigChooser {
        /**
         * Choose a configuration from the list. Implementors typically
         * implement this method by calling
         * {@link EGL10#eglChooseConfig} and iterating through the results. Please consult the
         * EGL specification available from The Khronos Group to learn how to call eglChooseConfig.
         * @param egl the EGL10 for the current display.
         * @param display the current display.
         * @return the chosen configuration.
         */
        EGLConfig chooseConfig(EGL10 egl, EGLDisplay display);
    }

    private abstract class BaseConfigChooser
            implements EGLConfigChooser {
        public BaseConfigChooser(int[] configSpec) {
            mConfigSpec = filterConfigSpec(configSpec);
        }

        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {
            int[] num_config = new int[1];
            if (!egl.eglChooseConfig(display, mConfigSpec, null, 0,
                    num_config)) {
                throw new IllegalArgumentException("eglChooseConfig failed");
            }

            int numConfigs = num_config[0];

            if (numConfigs <= 0) {
                throw new IllegalArgumentException(
                        "No configs match configSpec");
            }

            EGLConfig[] configs = new EGLConfig[numConfigs];
            if (!egl.eglChooseConfig(display, mConfigSpec, configs, numConfigs,
                    num_config)) {
                throw new IllegalArgumentException("eglChooseConfig#2 failed");
            }
            EGLConfig config = chooseConfig(egl, display, configs);
            if (config == null) {
                throw new IllegalArgumentException("No config chosen");
            }
            return config;
        }

        abstract EGLConfig chooseConfig(EGL10 egl, EGLDisplay display,
                EGLConfig[] configs);

        protected int[] mConfigSpec;

        private int[] filterConfigSpec(int[] configSpec) {
            if (mEGLContextClientVersion != 2 && mEGLContextClientVersion != 3) {
                return configSpec;
            }
            /* We know none of the subclasses define EGL_RENDERABLE_TYPE.
             * And we know the configSpec is well formed.
             */
            int len = configSpec.length;
            int[] newConfigSpec = new int[len + 2];
            System.arraycopy(configSpec, 0, newConfigSpec, 0, len-1);
            newConfigSpec[len-1] = EGL10.EGL_RENDERABLE_TYPE;
            if (mEGLContextClientVersion == 2) {
                newConfigSpec[len] = EGL14.EGL_OPENGL_ES2_BIT;  /* EGL_OPENGL_ES2_BIT */
            } else {
                newConfigSpec[len] = EGLExt.EGL_OPENGL_ES3_BIT_KHR; /* EGL_OPENGL_ES3_BIT_KHR */
            }
            newConfigSpec[len+1] = EGL10.EGL_NONE;
            return newConfigSpec;
        }
    }

    /**
     * Choose a configuration with exactly the specified r,g,b,a sizes,
     * and at least the specified depth and stencil sizes.
     */
    private class ComponentSizeChooser extends BaseConfigChooser {
        public ComponentSizeChooser(int redSize, int greenSize, int blueSize,
                int alphaSize, int depthSize, int stencilSize) {
            super(new int[] {
                    EGL10.EGL_RED_SIZE, redSize,
                    EGL10.EGL_GREEN_SIZE, greenSize,
                    EGL10.EGL_BLUE_SIZE, blueSize,
                    EGL10.EGL_ALPHA_SIZE, alphaSize,
                    EGL10.EGL_DEPTH_SIZE, depthSize,
                    EGL10.EGL_STENCIL_SIZE, stencilSize,
                    EGL10.EGL_NONE});
            mValue = new int[1];
            mRedSize = redSize;
            mGreenSize = greenSize;
            mBlueSize = blueSize;
            mAlphaSize = alphaSize;
            mDepthSize = depthSize;
            mStencilSize = stencilSize;
       }

        @Override
        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display,
                EGLConfig[] configs) {
            for (EGLConfig config : configs) {
                int d = findConfigAttrib(egl, display, config,
                        EGL10.EGL_DEPTH_SIZE, 0);
                int s = findConfigAttrib(egl, display, config,
                        EGL10.EGL_STENCIL_SIZE, 0);
                if ((d >= mDepthSize) && (s >= mStencilSize)) {
                    int r = findConfigAttrib(egl, display, config,
                            EGL10.EGL_RED_SIZE, 0);
                    int g = findConfigAttrib(egl, display, config,
                             EGL10.EGL_GREEN_SIZE, 0);
                    int b = findConfigAttrib(egl, display, config,
                              EGL10.EGL_BLUE_SIZE, 0);
                    int a = findConfigAttrib(egl, display, config,
                            EGL10.EGL_ALPHA_SIZE, 0);
                    if ((r == mRedSize) && (g == mGreenSize)
                            && (b == mBlueSize) && (a == mAlphaSize)) {
                        return config;
                    }
                }
            }
            return null;
        }

        private int findConfigAttrib(EGL10 egl, EGLDisplay display,
                EGLConfig config, int attribute, int defaultValue) {

            if (egl.eglGetConfigAttrib(display, config, attribute, mValue)) {
                return mValue[0];
            }
            return defaultValue;
        }

        private int[] mValue;
        // Subclasses can adjust these values:
        protected int mRedSize;
        protected int mGreenSize;
        protected int mBlueSize;
        protected int mAlphaSize;
        protected int mDepthSize;
        protected int mStencilSize;
        }

    /**
     * This class will choose a RGB_888 surface with
     * or without a depth buffer.
     *
     */
    private class SimpleEGLConfigChooser extends ComponentSizeChooser {
        public SimpleEGLConfigChooser(boolean withDepthBuffer) {
            super(8, 8, 8, 0, withDepthBuffer ? 16 : 0, 0);
        }
    }

    static class LogWriter extends Writer {

        @Override public void close() {
            flushBuilder();
        }

        @Override public void flush() {
            flushBuilder();
        }

        @Override public void write(char[] buf, int offset, int count) {
            for(int i = 0; i < count; i++) {
                char c = buf[offset + i];
                if ( c == '\n') {
                    flushBuilder();
                }
                else {
                    mBuilder.append(c);
                }
            }
        }

        private void flushBuilder() {
            if (mBuilder.length() > 0) {
                Log.v("GLSurfaceView", mBuilder.toString());
                mBuilder.delete(0, mBuilder.length());
            }
        }

        private StringBuilder mBuilder = new StringBuilder();
    }

    static class GLThreadManager {
        private static String TAG = "GLThreadManager";

        public synchronized void threadExiting(GLThread thread) {
            if (LOG_THREADS) {
                Log.i("GLThread", "exiting tid=" +  thread.getId());
            }
            if (mEglOwner == thread) {
                mEglOwner = null;
            }
            notifyAll();
        }

        /*
         * Tries once to acquire the right to use an EGL
         * context. Does not block. Requires that we are already
         * in the sGLThreadManager monitor when this is called.
         *
         * @return true if the right to use an EGL context was acquired.
         */
        public boolean tryAcquireEglContextLocked(GLThread thread) {
            if (mEglOwner == thread || mEglOwner == null) {
                mEglOwner = thread;
                notifyAll();
                return true;
            }
            checkGLESVersion();
            if (mMultipleGLESContextsAllowed) {
                return true;
            }
            // Notify the owning thread that it should release the context.
            // TODO: implement a fairness policy. Currently
            // if the owning thread is drawing continuously it will just
            // reacquire the EGL context.
            if (mEglOwner != null) {
                mEglOwner.requestReleaseEglContextLocked();
            }
            return false;
        }

        /*
         * Releases the EGL context. Requires that we are already in the
         * sGLThreadManager monitor when this is called.
         */
        public void releaseEglContextLocked(GLThread thread) {
            if (mEglOwner == thread) {
                mEglOwner = null;
            }
            notifyAll();
        }

        public synchronized boolean shouldReleaseEGLContextWhenPausing() {
            // Release the EGL context when pausing even if
            // the hardware supports multiple EGL contexts.
            // Otherwise the device could run out of EGL contexts.
            return mLimitedGLESContexts;
        }

        public synchronized boolean shouldTerminateEGLWhenPausing() {
            checkGLESVersion();
            return !mMultipleGLESContextsAllowed;
        }

        public synchronized void checkGLDriver(GL10 gl) {
            if (! mGLESDriverCheckComplete) {
                checkGLESVersion();
                String renderer = gl.glGetString(GL10.GL_RENDERER);
                if (mGLESVersion < kGLES_20) {
                    mMultipleGLESContextsAllowed =
                        ! renderer.startsWith(kMSM7K_RENDERER_PREFIX);
                    notifyAll();
                }
                mLimitedGLESContexts = !mMultipleGLESContextsAllowed;
                if (LOG_SURFACE) {
                    Log.w(TAG, "checkGLDriver renderer = \"" + renderer + "\" multipleContextsAllowed = "
                        + mMultipleGLESContextsAllowed
                        + " mLimitedGLESContexts = " + mLimitedGLESContexts);
                }
                mGLESDriverCheckComplete = true;
            }
        }

        private void checkGLESVersion() 
        {
            if (! mGLESVersionCheckComplete) 
            {
                try {
                	Class<?> SP = Class.forName("android.os.SystemProperties");
                	mGLESVersion = (Integer)SP.getMethod("getInt", String.class, int.class).invoke(null, "ro.opengles.version", ConfigurationInfo.GL_ES_VERSION_UNDEFINED);
                	if (mGLESVersion >= kGLES_20) 
                	{
                		mMultipleGLESContextsAllowed = true;
                	}
                } catch (Exception e) {
            	}
                if (LOG_SURFACE) {
                    Log.w(TAG, "checkGLESVersion mGLESVersion =" +
                            " " + mGLESVersion + " mMultipleGLESContextsAllowed = " + mMultipleGLESContextsAllowed);
                }
                mGLESVersionCheckComplete = true;
            }
        }

        /**
         * This check was required for some pre-Android-3.0 hardware. Android 3.0 provides
         * support for hardware-accelerated views, therefore multiple EGL contexts are
         * supported on all Android 3.0+ EGL drivers.
         */
        private boolean mGLESVersionCheckComplete;
        private int mGLESVersion;
        private boolean mGLESDriverCheckComplete;
        private boolean mMultipleGLESContextsAllowed;
        private boolean mLimitedGLESContexts;
        private static final int kGLES_20 = 0x20000;
        private static final String kMSM7K_RENDERER_PREFIX =
            "Q3Dimension MSM7500 ";
        private GLThread mEglOwner;
    }

    static final GLThreadManager sGLThreadManager = new GLThreadManager();

    private final WeakReference<GameSurface> mThisWeakRef = new WeakReference<GameSurface>(this);
    EGLConfigChooser mEGLConfigChooser;
    EGLContextFactory mEGLContextFactory;
    EGLWindowSurfaceFactory mEGLWindowSurfaceFactory;
    GLWrapper mGLWrapper;
    int mDebugFlags;
    int mEGLContextClientVersion;
    boolean mPreserveEGLContextOnPause;
    int mWidth;
    int mHeight;
    boolean mSizeChanged = true;
    boolean mRequestRender;
    boolean mRenderComplete;
    boolean mExited;
    boolean mPaused;
    boolean mHasSurface;
    boolean mSurfaceIsBad;
    boolean mHaveEglSurface;
    boolean mWaitingForSurface;
    boolean mFinishedCreatingEglSurface;
    boolean mHaveEglContext;
}