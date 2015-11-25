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

import android.annotation.TargetApi;
import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

public class GameLifecycleHandler
{
    public GameLifecycleHandler( Activity activity )
    {
    }
    
    @TargetApi( 11 )
    public void onCreateBegin( Bundle savedInstanceState )
    {
        Log.i( "GameLifecycleHandler", "onCreate" );
        
    }
    
    @TargetApi( 11 )
    public void onCreateEnd( Bundle savedInstanceState )
    {
    }
    
    public void onStart()
    {
        Log.i( "GameLifecycleHandler", "onStart" );
    }
    
    public void onResume()
    {
        Log.i( "GameLifecycleHandler", "onResume" );
    }
}
