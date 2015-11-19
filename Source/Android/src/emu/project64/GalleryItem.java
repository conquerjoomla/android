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


import android.content.Context;

public class GalleryItem
{
    public final boolean isHeading;
    
    public GalleryItem( Context context, String headingName )
    {
        this.isHeading = true;
    }
}
