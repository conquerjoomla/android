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
package emu.project64;


import emu.project64.task.CacheRomInfoService.CacheRomInfoListener;
import android.support.v4.app.Fragment;

public class CacheRomInfoFragment extends Fragment implements CacheRomInfoListener
{    
    //Progress dialog for ROM scan
    private ProgressDialog mProgress = null;
    
    @Override
    public void onCacheRomInfoFinished()
    {
    }
    
    @Override
    public void onCacheRomInfoServiceDestroyed()
    {
    }

    @Override
    public ProgressDialog GetProgressDialog()
    {
        return mProgress;
    }
}