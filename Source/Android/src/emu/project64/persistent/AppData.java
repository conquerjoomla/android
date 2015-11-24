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
package emu.project64.persistent;

import android.annotation.TargetApi;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Build;
import android.os.Environment;
public class AppData
{
    /** True if device is running Honeycomb or later (11 - Android 3.0.x) */
    public static final boolean IS_HONEYCOMB = Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB;
    
    /** Debug option: download data to SD card (default true). */
    public static final boolean DOWNLOAD_TO_SDCARD = true;
    
    /** The package name. */
    public final String packageName;
    /** The device's storage directory (typically the external storage directory). */
    private final String storageDir;
    
    /**
     * The subdirectory returned from the core's ConfigGetSharedDataPath() method. Location of extra
     * plugin-specific/cheats data. Contents deleted on uninstall.
     */
    public final String coreSharedDataDir;
    /** The object used to persist the settings. */
    private final SharedPreferences mPreferences;
    
    // Shared preferences keys
    private static final String KEY_ASSET_VERSION = "assetVersion";
    // Shared preferences default values
    private static final int DEFAULT_ASSET_VERSION = 0;
    /**
     * Instantiates a new object to retrieve and persist app data.
     * 
     * @param context The application context.
     */
    @TargetApi( 13 )
    public AppData( Context context )
    {
        packageName = context.getPackageName();
        if( DOWNLOAD_TO_SDCARD )
        {
            storageDir = Environment.getExternalStorageDirectory().getAbsolutePath();
            coreSharedDataDir = storageDir + "/Android/data/" + packageName;
        }
        else
        {
            storageDir = context.getFilesDir().getAbsolutePath();
            coreSharedDataDir = storageDir;
        }
        // Preference object for persisting app data
        String appDataFilename = packageName + "_appdata";
        mPreferences = context.getSharedPreferences( appDataFilename, Context.MODE_PRIVATE );
    }
    /**
     * Gets the asset version.
     * 
     * @return The asset version.
     */
    public int getAssetVersion()
    {
        return getInt( KEY_ASSET_VERSION, DEFAULT_ASSET_VERSION );
    }
    
    /**
     * Persists the asset version.
     * 
     * @param value The asset version.
     */
    public void putAssetVersion( int value )
    {
        putInt( KEY_ASSET_VERSION, value );
    }
    
    private int getInt( String key, int defaultValue )
    {
        return mPreferences.getInt( key, defaultValue );
    }
    
    private void putInt( String key, int value )
    {
        mPreferences.edit().putInt( key, value ).commit();
    }
}
