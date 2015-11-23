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

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import emu.project64.R;

import emu.project64.ActivityHelper;
import emu.project64.GalleryActivity;
import emu.project64.dialog.ProgressDialog;
import emu.project64.persistent.ConfigFile;
import emu.project64.util.RomHeader;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.os.Process;
import android.support.v4.app.NotificationCompat;
import android.text.TextUtils;
import android.util.Log;

public class CacheRomInfoService extends Service
{
    private String mSearchPath;
    private String mConfigPath;
    private boolean mClearGallery;
    private boolean mbStopped;
    
    private int mStartId;
    private Looper mServiceLooper;
    private ServiceHandler mServiceHandler;
    private final IBinder mBinder = new LocalBinder();
    private CacheRomInfoListener mListener = null;

    final static int ONGOING_NOTIFICATION_ID = 1;
    
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

    // Handler that receives messages from the thread
    private final class ServiceHandler extends Handler 
    {
        public ServiceHandler(Looper looper) 
        {
            super(looper);
        }
        
        @Override
        public void handleMessage(Message msg) 
        {
            Log.i( "CacheRomInfoService", "handleMessage start" );
            File searchPathFile = new File(mSearchPath);
            
            if( mSearchPath == null )
                throw new IllegalArgumentException( "Root path cannot be null" );
            if( !searchPathFile.exists() )
                throw new IllegalArgumentException( "Root path does not exist: " + searchPathFile.getAbsolutePath() );
            if( TextUtils.isEmpty( mConfigPath ) )
                throw new IllegalArgumentException( "Config file path cannot be null or empty" );
            final List<File> files = getAllFiles( searchPathFile );
            final ConfigFile config = new ConfigFile( mConfigPath );
            if (mClearGallery)
            {
                config.clear();
            }
            
            Log.i( "CacheRomInfoService", "scan start" );
            mListener.GetProgressDialog().setMaxProgress( files.size() );
            for( final File file : files )
            {
                mListener.GetProgressDialog().setMaxSubprogress( 0 );
                mListener.GetProgressDialog().setSubtext( "" );
                mListener.GetProgressDialog().setText( file.getAbsolutePath().substring( mSearchPath.length() ) );
                mListener.GetProgressDialog().setMessage( R.string.cacheRomInfo_searching );
                
                if( mbStopped ) break;
                RomHeader header = new RomHeader( file );
                if( header.isValid )
                {
                    cacheFile( file, config );
                }
                mListener.GetProgressDialog().incrementProgress( 1 );
            }
            config.save();
            Log.i( "CacheRomInfoService", "scan done" );
            
            if (mListener != null)
            {
                Log.i( "CacheRomInfoService", "calling onCacheRomInfoFinished" );
                mListener.onCacheRomInfoFinished();
            }

            // Stop the service using the startId, so that we don't stop
            // the service in the middle of handling another job
            stopSelf(msg.arg1);
        }
    }

    @Override
    public void onCreate() 
    {
		Log.i( "CacheRomInfoService", "onCreate start" );
		
		// Start up the thread running the service.  Note that we create a
		// separate thread because the service normally runs in the process's
		// main thread, which we don't want to block.  We also make it
		// background priority so CPU-intensive work will not disrupt our UI.
		HandlerThread thread = new HandlerThread("ServiceStartArguments",Process.THREAD_PRIORITY_BACKGROUND);
		thread.start();
		
		// Get the HandlerThread's Looper and use it for our Handler
		mServiceLooper = thread.getLooper();
		mServiceHandler = new ServiceHandler(mServiceLooper);
		
		//Show the notification
		Intent notificationIntent = new Intent(this, GalleryActivity.class);
		PendingIntent pendingIntent = PendingIntent.getActivity(this, 0, notificationIntent, 0);
		NotificationCompat.Builder builder = new NotificationCompat.Builder(this).setSmallIcon(R.drawable.icon)
		    .setContentTitle(getString(R.string.scanning_title))
		    .setContentText(getString(R.string.toast_pleaseWait))
		    .setContentIntent(pendingIntent);
		startForeground(ONGOING_NOTIFICATION_ID, builder.build());
		Log.i( "CacheRomInfoService", "onCreate Done" );
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) 
    {
        if(intent != null)
        {
            Bundle extras = intent.getExtras();
            mSearchPath = extras.getString( ActivityHelper.Keys.SEARCH_PATH );
            mConfigPath = extras.getString( ActivityHelper.Keys.CONFIG_PATH );
            mClearGallery = extras.getBoolean( ActivityHelper.Keys.CLEAR_GALLERY );
        }

        mbStopped = false;
        mStartId = startId;

        // If we get killed, after returning from here, restart
        return START_STICKY;
    }

    private List<File> getAllFiles( File searchPath )
    {
        List<File> result = new ArrayList<File>();
        if( searchPath.isDirectory() )
        {
            for( File file : searchPath.listFiles() )
            {
                if( mbStopped ) break;
                result.addAll( getAllFiles( file ) );
            }
        }
        else
        {
            result.add( searchPath );
        }
        return result;
    }
    
    private void cacheFile( File file, ConfigFile config)
    {
        if( mbStopped ) return;
        mListener.GetProgressDialog().setMessage( R.string.cacheRomInfo_computingMD5 );
        String md5 = ComputeMd5Task.computeMd5( file );
        RomHeader header = new RomHeader(file);
        
        if( mbStopped ) return;
        mListener.GetProgressDialog().setMessage( R.string.cacheRomInfo_searchingDB );
        config.put( md5, "romPath", file.getAbsolutePath() );
        config.put( md5, "crc", header.crc );
        config.put( md5, "headerName", header.name );
        
        String countryCodeString = Byte.toString(header.countryCode);
        config.put( md5, "countryCode",  countryCodeString);
        config.put( md5, "extracted", "false" );
        
        if( mbStopped ) return;
        mListener.GetProgressDialog().setMessage( R.string.cacheRomInfo_refreshingUI );
    }
    
    @Override
    public void onDestroy()
    {
        mbStopped = true;
        
        if (mListener != null)
        {
            mListener.onCacheRomInfoServiceDestroyed();
        }
    }

    @Override
    public IBinder onBind(Intent intent)
    {
        return mBinder;
    }
    
    public void SetCacheRomInfoListener(CacheRomInfoListener cacheRomInfoListener)
    {
        mListener = cacheRomInfoListener;
        mListener.GetProgressDialog().SetCacheRomInfoService(this);
        
        // For each start request, send a message to start a job and deliver the
        // start ID so we know which request we're stopping when we finish the job
        Message msg = mServiceHandler.obtainMessage();
        msg.arg1 = mStartId;
        mServiceHandler.sendMessage(msg);
    }

    public void Stop()
    {
        mbStopped = true;        
    }
}
