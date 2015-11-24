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
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.List;
import java.util.Locale;

import emu.project64.R;
import emu.project64.persistent.AppData;
import emu.project64.persistent.ConfigFile;
import emu.project64.persistent.ConfigFile.ConfigSection;
import emu.project64.persistent.GlobalPrefs;
import emu.project64.util.Notifier;
import emu.project64.util.RomHeader;
import android.annotation.TargetApi;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.app.FragmentManager;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

public class GalleryActivity extends AppCompatActivity
{
    // Saved instance states
    public static final String STATE_CACHE_ROM_INFO_FRAGMENT= "cache_rom_info_fragment";
    
    // App data and user preferences
    private AppData mAppData = null;
    private GlobalPrefs mGlobalPrefs = null;
    
    // Widgets
    private RecyclerView mGridView;
    private DrawerLayout mDrawerLayout;
    private ActionBarDrawerToggle mDrawerToggle;
    private MenuListView mDrawerList;

    // Searching
    private String mSearchQuery = "";
    
    // Resizable gallery thumbnails
    public int galleryWidth;
    public int galleryMaxWidth;
    public int galleryHalfSpacing;
    public int galleryColumns = 2;
    public float galleryAspectRatio;
    
    // Misc.
    private List<GalleryItem> mGalleryItems = null;
    private CacheRomInfoFragment mCacheRomInfoFragment = null;
    
    @Override
    protected void onNewIntent( Intent intent )
    {
        // If the activity is already running and is launched again (e.g. from a file manager app),
        // the existing instance will be reused rather than a new one created. This behavior is
        // specified in the manifest (launchMode = singleTask). In that situation, any activities
        // above this on the stack (e.g. GameActivity, GamePrefsActivity) will be destroyed
        // gracefully and onNewIntent() will be called on this instance. onCreate() will NOT be
        // called again on this instance. Currently, the only info that may be passed via the intent
        // is the selected game path, so we only need to refresh that aspect of the UI. This will
        // happen anyhow in onResume(), so we don't really need to do much here.
        super.onNewIntent( intent );
        
        // Only remember the last intent used
        setIntent( intent );
    }
    
    @Override
    protected void onCreate( Bundle savedInstanceState )
    {
        super.onCreate( savedInstanceState );
        
        // Get app data and user preferences
        mAppData = new AppData( this );
        mGlobalPrefs = new GlobalPrefs( this );
        mGlobalPrefs.enforceLocale( this );
        
        
        // Lay out the content
        setContentView( R.layout.gallery_activity );
        mGridView = (RecyclerView) findViewById( R.id.gridview );
        refreshGrid();
        
        // Update the grid layout
        galleryMaxWidth = (int) getResources().getDimension( R.dimen.galleryImageWidth );
        galleryHalfSpacing = (int) getResources().getDimension( R.dimen.galleryHalfSpacing );
        galleryAspectRatio = galleryMaxWidth * 1.0f
                / getResources().getDimension( R.dimen.galleryImageHeight );
        
        DisplayMetrics metrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics( metrics );
        
        int width = metrics.widthPixels - galleryHalfSpacing * 2;
        galleryColumns = (int) Math
                .ceil( width * 1.0 / ( galleryMaxWidth + galleryHalfSpacing * 2 ) );
        galleryWidth = width / galleryColumns - galleryHalfSpacing * 2;
        
        GridLayoutManager layoutManager = (GridLayoutManager) mGridView.getLayoutManager();
        layoutManager.setSpanCount( galleryColumns );
        // Add the toolbar to the activity (which supports the fancy menu/arrow animation)
        Toolbar toolbar = (Toolbar) findViewById( R.id.toolbar );
        toolbar.setTitle( R.string.app_name );
        setSupportActionBar( toolbar );
        
        // Configure the navigation drawer
        mDrawerLayout = (DrawerLayout) findViewById( R.id.drawerLayout );
        mDrawerToggle = new ActionBarDrawerToggle( this, mDrawerLayout, toolbar, 0, 0 )
        {
            @Override
            public void onDrawerStateChanged( int newState )
            {
            }
            
            @Override
            public void onDrawerClosed( View drawerView )
            {
            }
            
            @Override
            public void onDrawerOpened( View drawerView )
            {
            }
        };
        mDrawerLayout.setDrawerListener( mDrawerToggle );
        
        // Configure the list in the navigation drawer
        mDrawerList = (MenuListView) findViewById( R.id.drawerNavigation );
        mDrawerList.setMenuResource( R.menu.gallery_drawer );
        // Handle menu item selections
        mDrawerList.setOnClickListener( new MenuListView.OnClickListener()
        {
            @Override
            public void onClick( MenuItem menuItem )
            {
                GalleryActivity.this.onOptionsItemSelected( menuItem );
            }
        } );
        
        // find the retained fragment on activity restarts
        FragmentManager fm = getSupportFragmentManager();
        mCacheRomInfoFragment = (CacheRomInfoFragment) fm.findFragmentByTag(STATE_CACHE_ROM_INFO_FRAGMENT);
        
        if(mCacheRomInfoFragment == null)
        {
            mCacheRomInfoFragment = new CacheRomInfoFragment();
            fm.beginTransaction().add(mCacheRomInfoFragment, STATE_CACHE_ROM_INFO_FRAGMENT).commit();
        }
    }
    
    @Override
    public boolean onCreateOptionsMenu( Menu menu )
    {
        getMenuInflater().inflate( R.menu.gallery_activity, menu );
        
        return super.onCreateOptionsMenu( menu );
    }
    
    @Override
    public boolean onOptionsItemSelected( MenuItem item )
    {
        switch( item.getItemId() )
        {
            case R.id.menuItem_refreshRoms:
                ActivityHelper.StartRomScanService(this);
                return true;
            default:
                return super.onOptionsItemSelected( item );
        }
    }
    
    public void onGalleryItemClick( GalleryItem item )
    {
    }
    
    public boolean onGalleryItemLongClick( GalleryItem item )
    {
        launchGameActivity( item.romFile.getAbsolutePath(), item.md5, item.crc, item.headerName, item.countryCode, false );
        return true;
    }
    
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        // Check which request we're responding to
        if (requestCode == ActivityHelper.SCAN_ROM_REQUEST_CODE)
        {
            // Make sure the request was successful
            if (resultCode == RESULT_OK && data != null)
            {
                Bundle extras = data.getExtras();
                String searchPath = extras.getString( ActivityHelper.Keys.SEARCH_PATH );
                boolean clearGallery = extras.getBoolean( ActivityHelper.Keys.CLEAR_GALLERY );
                
                if (searchPath != null)
                {
                    refreshRoms(new File(searchPath), clearGallery);
                }
            }
        }
    }
    
    private void refreshRoms( final File startDir, boolean clearGallery )
    {
        mCacheRomInfoFragment.refreshRoms(startDir,  clearGallery, mAppData, mGlobalPrefs);
    }

    void refreshGrid( ){
        
        ConfigFile config = new ConfigFile( mGlobalPrefs.romInfoCache_cfg );
        
        String query = mSearchQuery.toLowerCase( Locale.US );
        String[] searches = null;
        if( query.length() > 0 )
            searches = query.split( " " );
        
        List<GalleryItem> items = new ArrayList<GalleryItem>();
        List<GalleryItem> recentItems = null;
        int currentTime = 0;
        if( mGlobalPrefs.isRecentShown )
        {
            recentItems = new ArrayList<GalleryItem>();
            currentTime = (int) ( new Date().getTime() / 1000 );
        }
        
        for( String md5 : config.keySet() )
        {
            if( !ConfigFile.SECTIONLESS_NAME.equals( md5 ) )
            {
                ConfigSection section = config.get( md5 );
                String goodName;
                if( mGlobalPrefs.isFullNameShown || !section.keySet().contains( "baseName" ) )
                    goodName = section.get( "goodName" );
                else
                    goodName = section.get( "baseName" );
                
                boolean matchesSearch = true;
                if( searches != null && searches.length > 0 )
                {
                    // Make sure the ROM name contains every token in the query
                    String lowerName = goodName.toLowerCase( Locale.US );
                    for( String search : searches )
                    {
                        if( search.length() > 0 && !lowerName.contains( search ) )
                        {
                            matchesSearch = false;
                            break;
                        }
                    }
                }
                
                if( matchesSearch )
                {
                    String romPath = config.get( md5, "romPath" );
                    String crc = config.get( md5, "crc" );
                    String headerName = config.get( md5, "headerName" );
                    String countryCodeString = config.get( md5, "countryCode" );
                    byte countryCode = 0;
                    
                    if(countryCodeString != null)
                    {
                        countryCode = Byte.parseByte(countryCodeString);
                    }
                    String lastPlayedStr = config.get( md5, "lastPlayed" );
                    String extracted = config.get( md5, "extracted" );
                    
                    if(crc == null || headerName == null || countryCodeString == null || extracted == null)
                    {
                        File file = new File(romPath);
                        RomHeader header = new RomHeader(file);
                        
                        crc = header.crc;
                        headerName = header.name;
                        countryCode = header.countryCode;
                        extracted = "false";
                        
                        config.put( md5, "crc", crc );
                        config.put( md5, "headerName", headerName );
                        config.put( md5, "countryCode", Byte.toString(countryCode) );
                        config.put( md5, "extracted", extracted );
                    }
                    
                    int lastPlayed = 0;
                    if( lastPlayedStr != null )
                    {
                        lastPlayed = Integer.parseInt( lastPlayedStr );
                    }
                    
                    GalleryItem item = new GalleryItem( this, md5, crc, headerName, countryCode, goodName, romPath, lastPlayed );
                    items.add( item );

                    if( mGlobalPrefs.isRecentShown
                            && currentTime - item.lastPlayed <= 60 * 60 * 24 * 7 ) // 7 days
                    {
                        recentItems.add( item );
                    }
                }
            }
        }
        
        config.save();

        Collections.sort( items, new GalleryItem.NameComparator() );
        if( recentItems != null )
            Collections.sort( recentItems, new GalleryItem.RecentlyPlayedComparator() );
        
        List<GalleryItem> combinedItems = items;
        if( mGlobalPrefs.isRecentShown && recentItems.size() > 0 )
        {
            combinedItems = new ArrayList<GalleryItem>();
            
            combinedItems.add( new GalleryItem( this, getString( R.string.galleryRecentlyPlayed ) ) );
            combinedItems.addAll( recentItems );
            
            combinedItems.add( new GalleryItem( this, getString( R.string.galleryLibrary ) ) );
            combinedItems.addAll( items );
            
            items = combinedItems;
        }
        
        mGalleryItems = items;
        mGridView.setAdapter( new GalleryItem.Adapter( this, items ) );
        
        // Allow the headings to take up the entire width of the layout
        final List<GalleryItem> finalItems = items;
        GridLayoutManager layoutManager = new GridLayoutManager( this, galleryColumns );
        layoutManager.setSpanSizeLookup( new GridLayoutManager.SpanSizeLookup()
        {
            @Override
            public int getSpanSize( int position )
            {
                // Headings will take up every span (column) in the grid
                if( finalItems.get( position ).isHeading )
                    return galleryColumns;
                
                // Games will fit in a single column
                return 1;
            }
        } );
        
        mGridView.setLayoutManager( layoutManager );
    }
    
    @Override
    protected void onResume()
    {
        super.onResume();
        refreshViews();
    }
    
    @TargetApi( 11 )
    private void refreshViews()
    {
        
        // Refresh the gallery
        refreshGrid();
    }
    
    public void launchGameActivity( String romPath, String romMd5, String romCrc, String romGoodName, byte romCountryCode, boolean isRestarting )
    {
        // Make sure that the storage is accessible
        if( !mAppData.isSdCardAccessible() )
        {
            Log.e( "GalleryActivity", "SD Card not accessible" );
            Notifier.showToast( this, R.string.toast_sdInaccessible );
            return;
        }
        
        // Notify user that the game activity is starting
        Notifier.showToast( this, R.string.toast_launchingEmulator );
        
        // Update the ConfigSection with the new value for lastPlayed
        String lastPlayed = Integer.toString( (int) ( new Date().getTime() / 1000 ) );
        ConfigFile config = new ConfigFile( mGlobalPrefs.romInfoCache_cfg );
        if( config != null )
        {
            config.put( romMd5, "lastPlayed", lastPlayed );
            
            
            config.save();
        }

        // Launch the game activity
        ActivityHelper.startGameActivity( this, romPath, romMd5, romCrc, romGoodName, romCountryCode,
                    isRestarting, mGlobalPrefs.isTouchpadEnabled );
    }
    
}
