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

import java.util.Locale;
import android.annotation.SuppressLint;
import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.preference.PreferenceManager;
import android.text.TextUtils;
import android.util.Log;

public class GlobalPrefs
{
    /** The parent directory containing all user-writable data files. */
    public final String userDataDir;
    
    /** The subdirectory containing gallery data cache. */
    public final String galleryCacheDir;
    
    /** The path of the rom info cache for the gallery. */
    public final String romInfoCache_cfg;
    
    /** True if Xperia Play touchpad is enabled. */
    public final boolean isTouchpadEnabled;
    
    /** True if the recently played section of the gallery should be shown. */
    public final boolean isRecentShown;
    
    /** True if the full ROM rip info should be shown. */
    public final boolean isFullNameShown;
    
    /** The screen orientation for the game activity. */
    public final int displayOrientation;
    
    /** The action bar transparency value. */
    public final int displayActionBarTransparency;
    /** True if big-screen navigation mode is enabled. */
    public final boolean isBigScreenMode;
    
    /** True if the action bar is available. */
    public final boolean isActionBarAvailable;
    
    private static final String KEY_LOCALE_OVERRIDE = "localeOverride";
    public static final String DEFAULT_LOCALE_OVERRIDE = "";
    private final SharedPreferences mPreferences;
    private final Locale mLocale;
    private final String mLocaleCode;
    /**
     * Instantiates a new user preferences wrapper.
     * 
     * @param context The application context.
     */
    @SuppressWarnings( "deprecation" )
    @SuppressLint( "InlinedApi" )
    @TargetApi( 17 )
    public GlobalPrefs( Context context )
    {
        AppData appData = new AppData( context );
        mPreferences = PreferenceManager.getDefaultSharedPreferences( context );
        
        // Locale
        mLocaleCode = mPreferences.getString( KEY_LOCALE_OVERRIDE, DEFAULT_LOCALE_OVERRIDE );
        mLocale = TextUtils.isEmpty( mLocaleCode ) ? Locale.getDefault() : createLocale( mLocaleCode );
        
        // Files
        userDataDir = mPreferences.getString( "pathGameSaves", "" );
        Log.i( "GlobalPrefs", "userDataDir = " + userDataDir );
        galleryCacheDir = userDataDir + "/GalleryCache";
        romInfoCache_cfg = galleryCacheDir + "/romInfoCache.cfg";
        
        // Library prefs
        isRecentShown = mPreferences.getBoolean( "showRecentlyPlayed", true );
        isFullNameShown = mPreferences.getBoolean( "showFullNames", true );
        // Xperia PLAY touchpad prefs
        isTouchpadEnabled = appData.hardwareInfo.isXperiaPlay && mPreferences.getBoolean( "touchpadEnabled", true );
        
        // Video prefs
        displayOrientation = getSafeInt( mPreferences, "displayOrientation", 0 );
        int transparencyPercent = mPreferences.getInt( "displayActionBarTransparency", 50 );
        displayActionBarTransparency = ( 255 * transparencyPercent ) / 100;
        
        // User interface modes
        String navMode = mPreferences.getString( "navigationMode", "auto" );
        if( navMode.equals( "bigscreen" ) )
        {
            isBigScreenMode = true;
        }
        else if( navMode.equals( "standard" ) )
        {
            isBigScreenMode = false;
        }
        else
        {
            isBigScreenMode = AppData.IS_OUYA_HARDWARE || appData.isAndroidTv; // TODO: Add other systems as they enter market
        }
        isActionBarAvailable = AppData.IS_HONEYCOMB && !isBigScreenMode;
        
    }
    
    public void enforceLocale( Activity activity )
    {
        Configuration config = activity.getBaseContext().getResources().getConfiguration();
        if( !mLocale.equals( config.locale ) )
        {
            config.locale = mLocale;
            activity.getBaseContext().getResources().updateConfiguration( config, null );
        }
    }
    
    private Locale createLocale( String code )
    {
        String[] codes = code.split( "_" );
        switch( codes.length )
        {
            case 1: // Language code provided
                return new Locale( codes[0] );
            case 2: // Language and country code provided
                return new Locale( codes[0], codes[1] );
            case 3: // Language, country, and variant code provided
                return new Locale( codes[0], codes[1], codes[2] );
            default: // Invalid input
                return null;
        }
    }
    
    /**
     * Gets the selected value of a ListPreference, as an integer.
     * 
     * @param preferences  The object containing the ListPreference.
     * @param key          The key of the ListPreference.
     * @param defaultValue The value to use if parsing fails.
     * 
     * @return The value of the selected entry, as an integer.
     */
    private static int getSafeInt( SharedPreferences preferences, String key, int defaultValue )
    {
        try
        {
            return Integer.parseInt( preferences.getString( key, String.valueOf( defaultValue ) ) );
        }
        catch( NumberFormatException ex )
        {
            return defaultValue;
        }
    }
}
