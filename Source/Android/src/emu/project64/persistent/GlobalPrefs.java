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

public class GlobalPrefs
{
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
        mPreferences = PreferenceManager.getDefaultSharedPreferences( context );
        
        // Locale
        mLocaleCode = mPreferences.getString( KEY_LOCALE_OVERRIDE, DEFAULT_LOCALE_OVERRIDE );
        mLocale = TextUtils.isEmpty( mLocaleCode ) ? Locale.getDefault() : createLocale( mLocaleCode );
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
}
