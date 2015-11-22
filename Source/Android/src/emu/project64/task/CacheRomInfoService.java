/****************************************************************************
*                                                                           *
* Project64 - A Nintendo 64 emulator.                                       *
* http://www.pj64-emu.com/                                                  *
* Copyright (C) 2012 Project64. All rights reserved.                        *
*                                                                           *
* License:                                                                  *
* GNU/GPLv2 http://www.gnu.org/licenses/gpl-2.0.html                        *
*                                                                           *
****************************************************************************/
package emu.project64.task;

import android.app.Service;
public class CacheRomInfoService extends Service
{
    private final IBinder mBinder = new LocalBinder();

    public interface CacheRomInfoListener
    {
        //This is called once the ROM scan is finished
        public void onCacheRomInfoFinished();
        
        //This is called when the service is destroyed
        public void onCacheRomInfoServiceDestroyed();
        
        //This is called to get a progress dialog object
        public ProgressDialog GetProgressDialog();
    }
    
    @Override
    public IBinder onBind(Intent intent)
    {
        return mBinder;
    }
}
