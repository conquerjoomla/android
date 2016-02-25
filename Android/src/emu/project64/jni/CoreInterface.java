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
package emu.project64.jni;

import java.io.File;
import java.util.ArrayList;

import emu.project64.R;
import emu.project64.game.GameSurface;
import emu.project64.persistent.AppData;
import emu.project64.persistent.GlobalPrefs;
import android.app.Activity;
import android.util.Log;

public class CoreInterface
{
    public interface OnStateCallbackListener
    {
        /**
         * Called when an emulator state/parameter has changed
         * 
         * @param paramChanged The parameter ID.
         * @param newValue The new value of the parameter.
         */
        public void onStateCallback( int paramChanged, int newValue );
    }
    
    // User/app data - used by NativeImports, NativeSDL
    protected static AppData sAppData = null;
    protected static GlobalPrefs sGlobalPrefs = null;
    private static Activity sActivity = null;
    
    // Startup info - used internally
    protected static String sRomPath = null;
    public static void initialize( Activity activity, String romPath, String romMd5, boolean isRestarting )
    {
        sRomPath = romPath;
        sActivity = activity;
        sAppData = new AppData( sActivity );
        sGlobalPrefs = new GlobalPrefs( sActivity );
        new File( sGlobalPrefs.coreUserDataDir ).mkdirs();
        new File( sGlobalPrefs.coreUserCacheDir ).mkdirs();
    }
    
    public static synchronized void startupEmulator(GameSurface.GLThread thread)
    {
		NativeExports.RunFileImage(sRomPath, thread);
    }
    
    public static synchronized void shutdownEmulator()
    {
    }
}
