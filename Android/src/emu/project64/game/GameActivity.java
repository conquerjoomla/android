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

import emu.project64.jni.CoreInterface;
import android.app.Activity;
import android.os.Bundle;

public class GameActivity extends Activity
{
    private GameLifecycleHandler mLifecycleHandler;
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
    
    @Override
    protected void onStart()
    {
        super.onStart();
        mLifecycleHandler.onStart();
    }
    
    @Override
    protected void onResume()
    {
        super.onResume();
        mLifecycleHandler.onResume();
    }
    
    @Override
    protected void onPause()
    {
        super.onPause();
        mLifecycleHandler.onPause();
    }
    
    @Override
    protected void onStop()
    {
        super.onStop();
        mLifecycleHandler.onStop();
    }
    
    @Override
    protected void onDestroy()
    {
        super.onDestroy();
        mLifecycleHandler.onDestroy();
    }
}
