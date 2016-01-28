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


import emu.project64.jni.CoreInterface.OnStateCallbackListener;
import android.annotation.TargetApi;
import android.app.Activity;

public class GameMenuHandler implements OnStateCallbackListener
{
    public GameMenuHandler( Activity activity )
    {
    }
    
    @TargetApi( 11 )
    @Override
    public void onStateCallback( int paramChanged, int newValue )
    {
    }
}
