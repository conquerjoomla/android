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

public class NativeExports
{
    static
    {
        System.loadLibrary( "Project64-bridge" );
    }
    
    public static native void appInit (String BaseDir );
    public static native void SettingsSaveBool (int type, boolean value );
    public static native void SettingsSaveString (int type, String value );
    
    
    
    
    
    public static native void loadLibraries( String libPath );
    
    public static native void unloadLibraries();
    
    public static native int emuStart( String userDataPath, String userCachePath, Object[] args );
}
