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

import emu.project64.R;
import emu.project64.jni.NativeExports;
import emu.project64.jni.SettingsID;
import emu.project64.persistent.AppData;
import emu.project64.persistent.GlobalPrefs;
import emu.project64.task.ExtractAssetsTask;
import emu.project64.task.ExtractAssetsTask.ExtractAssetsListener;
import emu.project64.task.ExtractAssetsTask.Failure;
import emu.project64.util.FileUtil;
import android.os.Bundle;
import android.os.Handler;
import android.preference.PreferenceManager;
import android.support.v7.app.AppCompatActivity;
import android.text.Html;
import android.util.Log;
import android.view.WindowManager.LayoutParams;
import android.widget.TextView;

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
    
    /** The total number of assets to be extracted (for computing progress %). */
    private static final int TOTAL_ASSETS = 120;
    
    /** The minimum duration that the splash screen is shown, in milliseconds. */
    private static final int SPLASH_DELAY = 2000;
    
    /**
     * The subdirectory within the assets directory to extract. A subdirectory is necessary to avoid
     * extracting all the default system assets in addition to ours.
     */
    private static final String SOURCE_DIR = "project64_data";
    
    /** The text view that displays extraction progress info. */
    private TextView mTextView;
    
    /** The running count of assets extracted. */
    private int mAssetsExtracted;
    
    // App data and user preferences
    private AppData mAppData = null;
    private GlobalPrefs mGlobalPrefs = null;
    
    @Override
    public void onCreate( Bundle savedInstanceState )
    {
        super.onCreate( savedInstanceState );
        
        // Get app data and user preferences
        mAppData = new AppData( this );
        mGlobalPrefs = new GlobalPrefs( this );
        mGlobalPrefs.enforceLocale( this );
        
        // Ensure that any missing preferences are populated with defaults (e.g. preference added to
        // new release)
        PreferenceManager.setDefaultValues( this, R.xml.preferences_global, false );
        
        // Refresh the preference data wrapper
        mGlobalPrefs = new GlobalPrefs( this );
        // Don't let the activity sleep in the middle of extraction
        getWindow().setFlags( LayoutParams.FLAG_KEEP_SCREEN_ON, LayoutParams.FLAG_KEEP_SCREEN_ON );
        
        // Lay out the content
        setContentView( R.layout.splash_activity );
        mTextView = (TextView) findViewById( R.id.mainText );
        
        // Extract the assets in a separate thread and launch the menu activity
        // Handler.postDelayed ensures this runs only after activity has resumed
        final Handler handler = new Handler();
        
        NativeExports.appInit(mAppData.coreSharedDataDir);
        NativeExports.SettingsSaveString(SettingsID.Directory_PluginSelected.getValue(), mAppData.libsDir);
        NativeExports.SettingsSaveBool(SettingsID.Directory_PluginUseSelected.getValue(), true);
        handler.postDelayed( extractAssetsTaskLauncher, SPLASH_DELAY );
    }
    
    /** Runnable that launches the non-UI thread from the UI thread after the activity has resumed. */
    private final Runnable extractAssetsTaskLauncher = new Runnable()
    {
        @Override
        public void run()
        {
        	Log.e( "Splash", "mAppData.coreSharedDataDir = " + mAppData.coreSharedDataDir);
            //if( !(new File( mAppData.coreSharedDataDir ) ).exists() || mAppData.getAssetVersion() != ASSET_VERSION )
            {
                // Extract and merge the assets if they are out of date
                //FileUtil.deleteFolder( new File( mAppData.coreSharedDataDir ) );
                mAssetsExtracted = 0;
                new ExtractAssetsTask( getAssets(), SOURCE_DIR, mAppData.coreSharedDataDir, SplashActivity.this ).execute();
            }
            //else
            {
                // Assets already extracted, just launch gallery activity, passing ROM path if it was provided externally
             //   ActivityHelper.startGalleryActivity( SplashActivity.this, getIntent().getData() );
                
                // We never want to come back to this activity, so finish it
              //  finish();
            }
        }
    };
    
    @Override
    public void onExtractAssetsProgress( String nextFileToExtract )
    {
        final float percent = ( 100f * mAssetsExtracted ) / (float) TOTAL_ASSETS;
        final String text = getString( R.string.assetExtractor_progress, percent, nextFileToExtract );
        mTextView.setText( text );
        mAssetsExtracted++;
    }
    
    @Override
    public void onExtractAssetsFinished( List<Failure> failures )
    {
        if( failures.size() == 0 )
        {
            // Extraction succeeded, record new asset version and merge cheats
            mTextView.setText( R.string.assetExtractor_finished );
            //mAppData.putAssetVersion( ASSET_VERSION );
            // Launch gallery activity, passing ROM path if it was provided externally
            ActivityHelper.startGalleryActivity( this, getIntent().getData() );
            
            // We never want to come back to this activity, so finish it
            finish();
        }
        else
        {
            // Extraction failed, update the on-screen text and don't start next activity
            String weblink = getResources().getString( R.string.assetExtractor_uriHelp );
            String message = getString( R.string.assetExtractor_failed, weblink );
            String textHtml = message.replace( "\n", "<br/>" ) + "<p><small>";
            for( Failure failure : failures )
            {
                textHtml += failure.toString() + "<br/>";
            }
            textHtml += "</small>";
            mTextView.setText( Html.fromHtml( textHtml ) );
        }
    }
}
