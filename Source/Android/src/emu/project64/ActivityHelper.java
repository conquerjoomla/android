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

import emu.project64.task.CacheRomInfoService;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.net.Uri;
import android.text.TextUtils;
import android.util.Log;

/**
 * Utility class that encapsulates and standardizes interactions between activities.
 */
public class ActivityHelper
{
    /**
     * Keys used to pass data to activities via the intent extras bundle. It's good practice to
     * namespace the keys to avoid conflicts with other apps. By convention this is usually the
     * package name but it's not a strict requirement. We'll use the fully qualified name of this
     * class since it's easy to get.
     */
    public static class Keys
    {
        private static final String NAMESPACE = Keys.class.getCanonicalName() + ".";
        //@formatter:off
        public static final String ROM_PATH             = NAMESPACE + "ROM_PATH";
        public static final String SEARCH_PATH          = NAMESPACE + "GALLERY_SEARCH_PATH";
        public static final String CONFIG_PATH          = NAMESPACE + "GALLERY_CONFIG_PATH";
        public static final String CLEAR_GALLERY        = NAMESPACE + "GALLERY_CLEAR_GALLERY";
        //@formatter:on
    }
    
    public static final int SCAN_ROM_REQUEST_CODE = 1;
    
    public static void startGalleryActivity( Context context, Uri romPath )
    {
        startGalleryActivity( context, romPath == null ? null : romPath.getPath() );
    }
    
    public static void startGalleryActivity( Context context, String romPath )
    {
        Intent intent = new Intent( context, GalleryActivity.class );
        if( !TextUtils.isEmpty( romPath ) )
        {
            intent.putExtra( ActivityHelper.Keys.ROM_PATH, romPath );
        }
        context.startActivity( intent );
    }
    
    public static void startGameActivity( Context context, String romPath, String romMd5, String romCrc,
            String romHeaderName, byte romCountryCode, boolean doRestart, boolean isXperiaPlay )
    {
    }
    
    public static void startCacheRomInfoService(Context context, ServiceConnection serviceConnection,
        String searchPath, String configPath, boolean clearGallery)
    {
        Log.i( "startCacheRomInfoService", "starting" );
        Intent intent = new Intent(context, CacheRomInfoService.class);
        intent.putExtra(ActivityHelper.Keys.SEARCH_PATH, searchPath);
        intent.putExtra(ActivityHelper.Keys.CONFIG_PATH, configPath);
        intent.putExtra(ActivityHelper.Keys.CLEAR_GALLERY, clearGallery);

        Log.i( "startCacheRomInfoService", "calling context.startService(intent)" );
        context.startService(intent);
        context.bindService(intent, serviceConnection, Context.BIND_AUTO_CREATE);
        Log.i( "startCacheRomInfoService", "done" );
    }
    
    public static void stopCacheRomInfoService(Context context, ServiceConnection serviceConnection)
    {
        Intent intent = new Intent(context, CacheRomInfoService.class);
        
        context.unbindService(serviceConnection);
        context.stopService(intent);
    }
    
    public static void StartRomScanService(Activity activity)
    {
        Intent intent = new Intent(activity, ScanRomsActivity.class);
        activity.startActivityForResult( intent, SCAN_ROM_REQUEST_CODE );
    }
}
