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
import emu.project64.persistent.GlobalPrefs;
import android.app.Activity;
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
    protected static GlobalPrefs sGlobalPrefs = null;
    private static Activity sActivity = null;
    private static Thread sCoreThread;
    
    // Startup info - used internally
    protected static String sRomPath = null;
    public static void initialize( Activity activity, GameSurface surface, String romPath, String romMd5, boolean isRestarting )
    {
        sRomPath = romPath;
        sActivity = activity;
        sGlobalPrefs = new GlobalPrefs( sActivity );
        new File( sGlobalPrefs.coreUserDataDir ).mkdirs();
        new File( sGlobalPrefs.coreUserCacheDir ).mkdirs();
    }
    
    public static void addOnStateCallbackListener( OnStateCallbackListener listener )
    {
    }
    
    public static synchronized void startupEmulator()
    {
        if( sCoreThread == null )
        {
            // Start the core thread if not already running
            sCoreThread = new Thread( new Runnable()
            {
                @Override
                public void run()
                {
                    // Initialize input-android plugin (even if we aren't going to use it)
                    ArrayList<String> arglist = new ArrayList<String>();
                    arglist.add( "projet64" );
                    arglist.add( sRomPath );
                    int result = NativeExports.emuStart( sGlobalPrefs.coreUserDataDir, sGlobalPrefs.coreUserCacheDir, arglist.toArray() );
                }
            }, "CoreThread" );
            
            // Start the core on its own thread
            sCoreThread.start();
        }
    }
}
