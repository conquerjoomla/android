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

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
/**
 * Utility class which collects a bunch of commonly used methods into one class.
 */
public final class Utility
{
    public static String executeShellCommand(String... args)
    {
        try
        {
            Process process = Runtime.getRuntime().exec( args );
            BufferedReader reader = new BufferedReader( new InputStreamReader( process.getInputStream() ) );
            StringBuilder result = new StringBuilder();
            String line;
            while( ( line = reader.readLine() ) != null )
            {
                result.append( line + "\n" );
            }
            return result.toString();
        }
        catch( IOException ignored )
        {
        }
        return "";
    }
}
