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

import emu.project64.R;

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
    public static void addOnStateCallbackListener( OnStateCallbackListener listener )
    {
    }
}
