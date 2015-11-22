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

import java.io.File;

import emu.project64.dialog.ProgressDialog;
import emu.project64.persistent.AppData;
import emu.project64.persistent.GlobalPrefs;
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
    
    public void refreshRoms( File startDir, boolean clearGallery, AppData appData, GlobalPrefs globalPrefs )
    {
    }
    
}