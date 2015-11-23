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
import emu.project64.task.CacheRomInfoService;
import emu.project64.task.CacheRomInfoService.CacheRomInfoListener;
import emu.project64.task.CacheRomInfoService.LocalBinder;
import android.app.Activity;
import android.support.v4.app.Fragment;
import android.content.ComponentName;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;

public class CacheRomInfoFragment extends Fragment implements CacheRomInfoListener
{    
    //Progress dialog for ROM scan
    private ProgressDialog mProgress = null;
    
    //Service connection for the progress dialog
    private ServiceConnection mServiceConnection;
    
    private AppData mAppData = null;
    
    private GlobalPrefs mGlobalPrefs = null;
    
    private boolean mCachedResult = false;
    
    private boolean mCachedRefreshRoms = false;
    
    private boolean mCachedScanFinish = false;
    
    private File mStartDir = null;
    private boolean mClearGallery = false;
    
    private boolean mInProgress = false;

    // this method is only called once for this fragment
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        // retain this fragment
        setRetainInstance(true);
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState)
    {
        super.onActivityCreated(savedInstanceState);
        
        if(mInProgress)
        {
            CharSequence title = getString( R.string.scanning_title );
            CharSequence message = getString( R.string.toast_pleaseWait );
            mProgress = new ProgressDialog( mProgress, getActivity(), title, mStartDir.getAbsolutePath(), message, true );
            mProgress.show();
        }
        
        if (mCachedResult && mInProgress)
        {
            ((GalleryActivity)getActivity()).refreshGrid();
            mCachedResult = false;
        }
        
        if(mCachedRefreshRoms)
        {
            ActuallyRefreshRoms(getActivity());
            mCachedRefreshRoms = false;
        }
        
        if(mCachedScanFinish)
        {
            ActivityHelper.stopCacheRomInfoService(getActivity().getApplicationContext(), mServiceConnection);
            mCachedScanFinish = false;
        }
    }

    @Override
    public void onAttach(Activity activity)
    {        
        super.onAttach(activity);
    }
    
    @Override
    public void onDetach()
    {
        //This can be null if this fragment is never utilized and this will be called on shutdown
        if(mProgress != null)
        {
            mProgress.dismiss();
        }
        
        super.onDetach();
    }
    
    @Override
    public void onDestroy()
    {        
        if(mServiceConnection != null)
        {
            ActivityHelper.stopCacheRomInfoService(getActivity().getApplicationContext(), mServiceConnection);
        }
        
        super.onDestroy();
    }

    @Override
    public void onCacheRomInfoFinished()
    {
        if(getActivity() != null)
        {
            ActivityHelper.stopCacheRomInfoService(getActivity().getApplicationContext(), mServiceConnection);
        }
        else
        {
            mCachedScanFinish = true;
        }

    }
    
    @Override
    public void onCacheRomInfoServiceDestroyed()
    {
        mInProgress = false;
        
        if(getActivity() != null)
        {
            ((GalleryActivity)getActivity()).refreshGrid();
            mProgress.dismiss();
        }
        else
        {
            mCachedResult = true;
        }
    }

    @Override
    public ProgressDialog GetProgressDialog()
    {
        return mProgress;
    }

    public void refreshRoms( File startDir, boolean clearGallery, AppData appData, GlobalPrefs globalPrefs )
    {
        this.mStartDir = startDir;
        this.mClearGallery = clearGallery;
        this.mAppData = appData;
        this.mGlobalPrefs = globalPrefs;
        
        if(getActivity() != null)
        {
            ActuallyRefreshRoms(getActivity());
        }
        else
        {
            mCachedRefreshRoms = true;
        }
    }
    
    private void ActuallyRefreshRoms(Activity activity)
    {
        Log.i( "ActuallyRefreshRoms", "starting" );
        mInProgress = true;
        
        CharSequence title = getString( R.string.scanning_title );
        CharSequence message = getString( R.string.toast_pleaseWait );
        mProgress = new ProgressDialog( mProgress, getActivity(), title, mStartDir.getAbsolutePath(), message, true );
        mProgress.show();
        
        /** Defines callbacks for service binding, passed to bindService() */
        mServiceConnection = new ServiceConnection() 
        {
            @Override
            public void onServiceConnected(ComponentName className, IBinder service) 
            {
                Log.i( "ActuallyRefreshRoms", "onServiceConnected starting" );
                // We've bound to LocalService, cast the IBinder and get LocalService instance
                LocalBinder binder = (LocalBinder) service;
                CacheRomInfoService cacheRomInfoService = binder.getService();
                cacheRomInfoService.SetCacheRomInfoListener(CacheRomInfoFragment.this);
                Log.i( "ActuallyRefreshRoms", "onServiceConnected Done" );
            }

            @Override
            public void onServiceDisconnected(ComponentName arg0) 
            {
                //Nothing to do here
            }
        };

        // Asynchronously search for ROMs
		ActivityHelper.startCacheRomInfoService(activity.getApplicationContext(), mServiceConnection,
            mStartDir.getAbsolutePath(), mGlobalPrefs.romInfoCache_cfg, mClearGallery);
        Log.i( "ActuallyRefreshRoms", "Done" );
    }
}