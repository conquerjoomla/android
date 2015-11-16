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
package emu.project64.task;

import java.util.ArrayList;
import java.util.List;
import android.content.res.AssetManager;
import android.os.AsyncTask;
import android.text.TextUtils;

public class ExtractAssetsTask extends AsyncTask<Void, String, List<ExtractAssetsTask.Failure>>
{
    public interface ExtractAssetsListener
    {
        public void onExtractAssetsProgress( String nextFileToExtract );
        public void onExtractAssetsFinished( List<Failure> failures );
    }
    
    public ExtractAssetsTask( AssetManager assetManager, String srcPath, String dstPath, ExtractAssetsListener listener )
    {
        if (assetManager == null )
            throw new IllegalArgumentException( "Asset manager cannot be null" );
        if( TextUtils.isEmpty( srcPath ) )
            throw new IllegalArgumentException( "Source path cannot be null or empty" );
        if( TextUtils.isEmpty( dstPath ) )
            throw new IllegalArgumentException( "Destination path cannot be null or empty" );
        if( listener == null )
            throw new IllegalArgumentException( "Listener cannot be null" );
        
        mSrcPath = srcPath;
        mDstPath = dstPath;
    }
    
    private final String mSrcPath;
    private final String mDstPath;
    
    @Override
    protected List<Failure> doInBackground( Void... params )
    {
        return extractAssets( mSrcPath, mDstPath );
    }
    public static final class Failure
    {
    }
    
    private List<Failure> extractAssets( String srcPath, String dstPath )
    {
        final List<Failure> failures = new ArrayList<Failure>();
        return failures;
    }
}
