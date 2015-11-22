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

import emu.project64.dialog.ProgressDialog;
import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;

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

    /**
     * Class used for the client Binder.  Because we know this service always
     * runs in the same process as its clients, we don't need to deal with IPC.
     */
    public class LocalBinder extends Binder 
    {
        public CacheRomInfoService getService() 
        {
            // Return this instance of CacheRomInfoService so clients can call public methods
            return CacheRomInfoService.this;
        }
    }
    
    @Override
    public IBinder onBind(Intent intent)
    {
        return mBinder;
    }

    public void Stop()
    {
    }
}
