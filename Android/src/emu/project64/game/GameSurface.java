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

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceView;

public class GameSurface extends GLSurfaceView2
{
	private boolean rendererSet;

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

			//this.setEGLContextClientVersion(2);
			this.setRenderer(new RendererWrapper());
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
}