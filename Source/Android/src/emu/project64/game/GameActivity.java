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
import android.os.Bundle;

public class GameActivity extends Activity
{
    private GameMenuHandler mMenuHandler;
    
    @Override
    protected void onCreate( Bundle savedInstanceState )
    {
        mMenuHandler = new GameMenuHandler( this );
        CoreInterface.addOnStateCallbackListener( mMenuHandler  );
        
        mLifecycleHandler = new GameLifecycleHandler( this );
        mLifecycleHandler.onCreateBegin( savedInstanceState );
        super.onCreate( savedInstanceState );
        mLifecycleHandler.onCreateEnd( savedInstanceState );
    }
}
