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

import java.lang.ref.WeakReference;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import emu.project64.R;
import emu.project64.ActivityHelper;
import emu.project64.game.GLThread;
import emu.project64.jni.CoreInterface;
import emu.project64.jni.NativeConstants;
import emu.project64.jni.NativeExports;
import emu.project64.jni.NativeXperiaTouchpad;
import emu.project64.persistent.AppData;
import emu.project64.persistent.GlobalPrefs;
import android.annotation.SuppressLint;
import android.annotation.TargetApi;
import android.app.Activity;
import android.app.ActivityManager;
import android.content.Context;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.text.TextUtils;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.Window;
import android.view.WindowManager.LayoutParams;

public class GameLifecycleHandler implements SurfaceHolder.Callback, GLThread.SurfaceInfo
{
	// Activity and views
	private Activity mActivity;
	private GameSurface mSurface;
	private GameOverlay mOverlay;
	// Internal flags
	private final boolean mIsXperiaPlay;

	// Intent data
	private final String mRomPath;
	private final String mRomMd5;
	private final String mRomCrc;
	private final String mRomHeaderName;
	private final byte mRomCountryCode;
	private final boolean mDoRestart;

	// Lifecycle state tracking
	private boolean mIsFocused = false; // true if the window is focused
	private boolean mIsResumed = false; // true if the activity is resumed
	private boolean mIsSurface = false; // true if the surface is available

	// App data and user preferences
	private GlobalPrefs mGlobalPrefs;
        
	public GameLifecycleHandler(Activity activity) 
	{
		mActivity = activity;
		mIsXperiaPlay = !(activity instanceof GameActivity);

		// Get the intent data
		Bundle extras = mActivity.getIntent().getExtras();
		if (extras == null)
		{
			throw new Error("ROM path and MD5 must be passed via the extras bundle when starting GameActivity");
		}
		mRomPath = extras.getString(ActivityHelper.Keys.ROM_PATH);
		mRomMd5 = extras.getString(ActivityHelper.Keys.ROM_MD5);
		mRomCrc = extras.getString(ActivityHelper.Keys.ROM_CRC);
		mRomHeaderName = extras.getString(ActivityHelper.Keys.ROM_HEADER_NAME);
		mRomCountryCode = extras.getByte(ActivityHelper.Keys.ROM_COUNTRY_CODE);
		mDoRestart = extras.getBoolean(ActivityHelper.Keys.DO_RESTART, false);
		if (TextUtils.isEmpty(mRomPath) || TextUtils.isEmpty(mRomMd5)) 
		{
			throw new Error( "ROM path and MD5 must be passed via the extras bundle when starting GameActivity");
		}
	}

	@TargetApi(11)
	public void onCreateBegin(Bundle savedInstanceState) 
	{
		Log.i("GameLifecycleHandler", "onCreate");
		// Get app data and user preferences
		mGlobalPrefs = new GlobalPrefs(mActivity);

		mGlobalPrefs.enforceLocale(mActivity);

		// For Honeycomb, let the action bar overlay the rendered view (rather
		// than squeezing it)
		// For earlier APIs, remove the title bar to yield more space
		Window window = mActivity.getWindow();
		if (mGlobalPrefs.isActionBarAvailable) 
		{
			window.requestFeature(Window.FEATURE_ACTION_BAR_OVERLAY);
		} 
		else 
		{
			window.requestFeature(Window.FEATURE_NO_TITLE);
		}

		// Enable full-screen mode
		window.setFlags(LayoutParams.FLAG_FULLSCREEN, LayoutParams.FLAG_FULLSCREEN);

		// Keep screen from going to sleep
		window.setFlags(LayoutParams.FLAG_KEEP_SCREEN_ON, LayoutParams.FLAG_KEEP_SCREEN_ON);

		// Set the screen orientation
		mActivity.setRequestedOrientation(mGlobalPrefs.displayOrientation);

		// If the orientation changes, the screensize info changes, so we must
		// refresh dependencies
		mGlobalPrefs = new GlobalPrefs(mActivity);
	}

	@TargetApi(11)
	public void onCreateEnd(Bundle savedInstanceState) 
	{
		// Take control of the GameSurface if necessary
		if (mIsXperiaPlay)
			mActivity.getWindow().takeSurface(null);

		// Lay out content and get the views
		mActivity.setContentView(R.layout.game_activity);
        mSurface = (GameSurface) mActivity.findViewById( R.id.gameSurface );
		mOverlay = (GameOverlay) mActivity.findViewById(R.id.gameOverlay);

		// Initialize the objects and data files interfacing to the emulator
		// core
		CoreInterface.initialize(mActivity, mRomPath, mRomMd5, mDoRestart);
        
        // Listen to game surface events (created, changed, destroyed)
        mSurface.getHolder().addCallback( this );
        mSurface.createGLContext((ActivityManager)mActivity.getSystemService(Context.ACTIVITY_SERVICE));

        /*mSurface.getHolder().setFixedSize( mGlobalPrefs.videoRenderWidth, mGlobalPrefs.videoRenderHeight );
        FrameLayout.LayoutParams params = (FrameLayout.LayoutParams) mSurface.getLayoutParams();
        params.width = mGlobalPrefs.videoSurfaceWidth;
        params.height = mGlobalPrefs.videoSurfaceHeight;
        params.gravity = mGlobalPrefs.displayPosition | Gravity.CENTER_HORIZONTAL;
        mSurface.setLayoutParams( params );*/

		// Configure the action bar introduced in higher Android versions
		if (mGlobalPrefs.isActionBarAvailable) 
		{
			mActivity.getActionBar().hide();
			ColorDrawable color = new ColorDrawable(Color.parseColor("#303030"));
			color.setAlpha(mGlobalPrefs.displayActionBarTransparency);
			mActivity.getActionBar().setBackgroundDrawable(color);
		}

		// Initialize user interface devices
		View inputSource = mIsXperiaPlay ? new NativeXperiaTouchpad(mActivity) : mOverlay;
		initControllers(inputSource);
	}

	public void onStart() 
	{
		Log.i("GameLifecycleHandler", "onStart");
	}

	public void onResume() 
	{
		Log.i("GameLifecycleHandler", "onResume");
		mIsResumed = true;
		tryRunning();
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) 
	{
		Log.i("GameLifecycleHandler", "surfaceCreated");
	}

	@Override
	public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) 
	{
		Log.i("GameLifecycleHandler", "surfaceChanged");
		mIsSurface = true;
		tryRunning();
	}

	public void onWindowFocusChanged(boolean hasFocus) {
		// Only try to run; don't try to pause. User may just be touching the
		// in-game menu.
		Log.i("GameLifecycleHandler", "onWindowFocusChanged: " + hasFocus);
		mIsFocused = hasFocus;
		if (hasFocus) {
			hideSystemBars();
		}
		tryRunning();
	}

	public void onPause() {
		Log.i("GameLifecycleHandler", "onPause");
		mIsResumed = false;
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		Log.i("GameLifecycleHandler", "surfaceDestroyed");
		mIsSurface = false;
	}

	public void onStop() {
		Log.i("GameLifecycleHandler", "onStop");
	}

	public void onDestroy() {
		Log.i("GameLifecycleHandler", "onDestroy");
	}

	@SuppressLint("InlinedApi")
	private void initControllers(View inputSource) {
	}

	@SuppressLint("InlinedApi")
	@TargetApi(11)
	private void hideSystemBars() 
	{
	}

	private boolean isSafeToRender() 
	{
		return mIsFocused && mIsResumed && mIsSurface;
	}

	private void tryRunning() 
	{
		int state = NativeExports.emuGetState();
		if (isSafeToRender() && (state != NativeConstants.EMULATOR_STATE_RUNNING)) 
		{
			switch (state) 
			{
			case NativeConstants.EMULATOR_STATE_IDLE:
				CoreInterface.startupEmulator(new GLThread(new WeakReference<GameSurface>(mSurface), this));
				break;
			case NativeConstants.EMULATOR_STATE_PAUSED:
				break;
			default:
				break;
			}
		}
	}
	
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
}
