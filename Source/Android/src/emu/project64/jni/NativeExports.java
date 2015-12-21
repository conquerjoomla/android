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
        System.loadLibrary( "ae-exports" );
    }
    
    public static native void loadLibraries( String libPath );
    
    public static native void unloadLibraries();
    
    public static native int emuStart( String userDataPath, String userCachePath, Object[] args );
}