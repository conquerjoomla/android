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
package emu.project64.util;

import java.io.File;

/**
 * Utility class that provides methods which simplify file I/O tasks.
 */
public final class FileUtil
{
    /**
     * Deletes a given folder directory in the form of a {@link File}
     * 
     * @param folder The folder to delete.
     * 
     * @return True if the folder was deleted, false otherwise.
     */
    public static boolean deleteFolder( File folder )
    {
        if( folder.isDirectory() )
        {
            String[] children = folder.list();
            if( children != null )
            {
                for( String child : children )
                {
                    boolean success = deleteFolder( new File( folder, child ) );
                    if( !success )
                        return false;
                }
            }
        }
        
        return folder.delete();
    }
    
}
