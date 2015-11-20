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

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;

/**
 * Utility class that encapsulates and standardizes interactions between activities.
 */
public class ActivityHelper
{
    public static final int SCAN_ROM_REQUEST_CODE = 1;
    
    public static void startGalleryActivity( Context context, Uri romPath )
    {
        startGalleryActivity( context, romPath == null ? null : romPath.getPath() );
    }
    
    public static void startGalleryActivity( Context context, String romPath )
    {
        Intent intent = new Intent( context, GalleryActivity.class );
        context.startActivity( intent );
    }
    
    public static void StartRomScanService(Activity activity)
    {
        Intent intent = new Intent(activity, ScanRomsActivity.class);
        activity.startActivityForResult( intent, SCAN_ROM_REQUEST_CODE );
    }
}
