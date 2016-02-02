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

import android.app.Activity;
import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;

public class GameSurface extends Activity 
{
	private GLSurfaceView glSurfaceView;
	private boolean rendererSet;

	@Override
	protected void onCreate(Bundle savedInstanceState) 
	{
		super.onCreate(savedInstanceState);

		ActivityManager activityManager = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
		ConfigurationInfo configurationInfo = activityManager.getDeviceConfigurationInfo();

		final boolean supportsEs2 = configurationInfo.reqGlEsVersion >= 0x20000 || isProbablyEmulator();

		if (supportsEs2) 
		{
			glSurfaceView = new GLSurfaceView(this);

			if (isProbablyEmulator()) 
			{
				// Avoids crashes on startup with some emulator images.
				glSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
			}

			glSurfaceView.setEGLContextClientVersion(2);
			glSurfaceView.setRenderer(new RendererWrapper());
			rendererSet = true;
			setContentView(glSurfaceView);
		}
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

	@Override
	protected void onPause() 
	{
		super.onPause();

		if (rendererSet) 
		{
			glSurfaceView.onPause();
		}
	}

	@Override
	protected void onResume() 
	{
		super.onResume();

		if (rendererSet) 
		{
			glSurfaceView.onResume();
		}
	}
}