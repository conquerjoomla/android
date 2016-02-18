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

import emu.project64.game.GLThread;
import emu.project64.game.GameSurface;

public class NativeExports
{
    static
    {
        System.loadLibrary( "Project64-bridge" );
    }
    
    public static native void appInit (String BaseDir );
    public static native void SettingsSaveBool (int type, boolean value );
    public static native void SettingsSaveString (int type, String value );
    public static native void RunFileImage( String FileLoc, GLThread thread );
    
	public static native int emuGetState();
	
    public static native void onSurfaceCreated();    
    public static native void onSurfaceChanged(int width, int height); 
    public static native void onDrawFrame();
}
