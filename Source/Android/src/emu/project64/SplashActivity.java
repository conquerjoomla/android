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
package emu.project64;

import java.io.File;
import java.util.List;

import emu.project64.persistent.AppData;
import emu.project64.task.ExtractAssetsTask;
import emu.project64.task.ExtractAssetsTask.ExtractAssetsListener;
import emu.project64.task.ExtractAssetsTask.Failure;
import emu.project64.util.FileUtil;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.view.WindowManager.LayoutParams;

/**
 * The main activity that presents the splash screen, extracts the assets if necessary, and launches
 * the main menu activity.
 */
public class SplashActivity extends AppCompatActivity implements ExtractAssetsListener
{
    /**
     * Asset version number, used to determine stale assets. Increment this number every time the
     * assets are updated on disk.
     */
    private static final int ASSET_VERSION = 1;
    
    /** The minimum duration that the splash screen is shown, in milliseconds. */
    private static final int SPLASH_DELAY = 1000;
    
    /**
     * The subdirectory within the assets directory to extract. A subdirectory is necessary to avoid
     * extracting all the default system assets in addition to ours.
     */
    private static final String SOURCE_DIR = "project64_data";
    
    /** The running count of assets extracted. */
    private int mAssetsExtracted;
    
    // App data and user preferences
    private AppData mAppData = null;
    
    @Override
    public void onCreate( Bundle savedInstanceState )
    {
        super.onCreate( savedInstanceState );
        
        // Get app data and user preferences
        mAppData = new AppData( this );
        // Don't let the activity sleep in the middle of extraction
        getWindow().setFlags( LayoutParams.FLAG_KEEP_SCREEN_ON, LayoutParams.FLAG_KEEP_SCREEN_ON );
        
        // Lay out the content
        setContentView( R.layout.splash_activity );
        
        // Extract the assets in a separate thread and launch the menu activity
        // Handler.postDelayed ensures this runs only after activity has resumed
        final Handler handler = new Handler();
        handler.postDelayed( extractAssetsTaskLauncher, SPLASH_DELAY );
    }
    
    /** Runnable that launches the non-UI thread from the UI thread after the activity has resumed. */
    private final Runnable extractAssetsTaskLauncher = new Runnable()
    {
        @Override
        public void run()
        {        	
            if( !(new File( mAppData.coreSharedDataDir ) ).exists() || mAppData.getAssetVersion() != ASSET_VERSION )
            {
                // Extract and merge the assets if they are out of date
                FileUtil.deleteFolder( new File( mAppData.coreSharedDataDir ) );
                mAssetsExtracted = 0;
                new ExtractAssetsTask( getAssets(), SOURCE_DIR, mAppData.coreSharedDataDir, SplashActivity.this ).execute();
            }
            else
            {
            }
        }
    };
    
    @Override
    public void onExtractAssetsProgress( String nextFileToExtract )
    {
    }
    
    @Override
    public void onExtractAssetsFinished( List<Failure> failures )
    {
    }
}
