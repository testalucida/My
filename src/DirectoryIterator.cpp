#include <my/DirectoryIterator.h>

#include <my/StringHelper.h>

#ifdef WIN32
#include <io.h>
#else
#include <dirent.h>
#include <sys/types.h>
#endif
#include <string.h>
#include <time.h>
#include <algorithm>
#include <iostream>

#ifdef WIN32
static const char *SLASH = "\\";
#else
static const char *SLASH = "/";
#endif

using namespace std;

bool sortEntries( my::EntryPtr p1, my::EntryPtr p2 ) {
	if( p1->isDirectory && !p2->isDirectory ) return true;
	if( !p1->isDirectory && p2->isDirectory ) return false;

	int rc = strcmp( p1->directory.c_str(), p2->directory.c_str() );
	if( rc == 0 ) {
		rc = strcmp( p1->name.c_str(), p2->name.c_str() );
	}

	return ( rc < 0 );
}

namespace my {

DirectoryIterator::DirectoryIterator()
	: _startDir( "." ),
	  _includeSubDirs( true ),
	  _stop( false ),
	  _pMasterStop( &_stop ),
	  _sort( true )
{
}

DirectoryIterator::DirectoryIterator( const std::string &startDir, bool includeSubDirs, bool sort )
	: _startDir( startDir ),
	  _includeSubDirs( includeSubDirs ),
	  _stop( false )
	  ,_pMasterStop( &_stop )
	  ,_sort( sort )
{
}

DirectoryIterator::DirectoryIterator( const std::string &startDir, bool includeSubDirs, bool sort, bool *pStop )
	: _startDir( startDir ),
	  _includeSubDirs( includeSubDirs ),
	  _stop( false ),
	  _pMasterStop( pStop ),
	  _sort( sort )
{
}

DirectoryIterator::~DirectoryIterator() {
}

void DirectoryIterator::iterate() {
	iterate( _startDir, "*.*" );
    if( !_stop ) {
        if( _sort ) {
            std::sort( _entries.begin(), _entries.end(), sortEntries );

            for_each( _entries.begin(), _entries.end(), [&]( EntryPtr pEntry ) {
                if( !_stop ) {
                    EntryFoundEvent efe = {pEntry};
                    signalEntryFound.send( this, &efe );
                }
            } );
        }
    }
	NULLPARM nil;
	signalTerminated.send( this, &nil );
}

void DirectoryIterator::iterate( const string &dir, const string & filePattern ) {
    //BULLSHIT! filepattern is always wildcard
	//if( *_pMasterStop ) {
    if( _stop ) {
		return;
	}

	string searchFor = dir;
	if( !StringHelper::instance().endsWithSlash( searchFor ) ) {
		searchFor.append( SLASH );
	}
	searchFor.append( filePattern );
#ifdef WIN32
	struct _finddata_t c_file;
	intptr_t hFile;

	if( ( hFile = _findfirst( searchFor.c_str(), &c_file ) ) > -1L )  {
		do {
			if( signalEntryFound.getConnectionCount() > 0 &&
				!strcmp( c_file.name, "." ) == 0 &&
				!strcmp( c_file.name, ".." ) == 0 )
			{
				EntryPtr pEntry( new Entry() );
				pEntry->directory = dir;
				pEntry->isArchived =  ( c_file.attrib & _A_ARCH )   ? true : false;
				pEntry->isDirectory =  ( c_file.attrib & _A_SUBDIR )   ? true : false;
				pEntry->isHidden =  ( c_file.attrib & _A_HIDDEN )   ? true : false;
				pEntry->isReadOnly =  ( c_file.attrib & _A_RDONLY )   ? true : false;
				pEntry->isSystem =  ( c_file.attrib & _A_SYSTEM )   ? true : false;
				pEntry->name = c_file.name;
				ctime_s( pEntry->lastWrite, _countof( pEntry->lastWrite ), &c_file.time_write );
				pEntry->size = c_file.size;
				if( _sort ) {
					rememberEntry( pEntry );
				} else {
					//send( this, &pEntry );
					EntryFoundEvent efe = {pEntry};
					signalEntryFound.send( this, &efe );
				}
				if( _includeSubDirs && pEntry->isDirectory ) {
					string subdir = dir;
					if( !StringHelper::instance().endsWithSlash( subdir ) ) {
						subdir.append( "\\" );
					}
					subdir.append( pEntry->name );
					iterate( subdir, filePattern );
				}
			}
		} while( _findnext( hFile, &c_file ) == 0 );

		_findclose( hFile );
	}
#else
    DIR *dp = opendir( dir.c_str() );
    if( dp ) {
        struct dirent *ep;
        while( !_stop && ( ep = readdir( dp ) ) ) {
            if(	strcmp( ep->d_name, "." ) != 0 &&
				strcmp( ep->d_name, ".." ) != 0 )
			{
				EntryPtr pEntry( new Entry() );
				pEntry->directory = dir;
				pEntry->isArchived =  false; //don't know
				pEntry->isDirectory =  ( ep->d_type == DT_DIR );
                pEntry->isNormalFile = ( ep->d_type == DT_REG );
				pEntry->isHidden = false;
				pEntry->isReadOnly =  false;
				pEntry->isSystem = false;
				pEntry->name = ep->d_name;
				pEntry->size = ep->d_reclen;
				if( _sort ) {
					rememberEntry( pEntry );
				} else {
					//send( this, &pEntry );
					EntryFoundEvent efe = {pEntry};
					signalEntryFound.send( this, &efe );
				}
				if( _includeSubDirs && pEntry->isDirectory ) {
					string subdir = dir;
					if( !StringHelper::instance().endsWithSlash( subdir ) ) {
						subdir.append( SLASH );
					}
					subdir.append( pEntry->name );
					iterate( subdir, filePattern );
				}
			}
        }
        closedir( dp );
    }
#endif
}

void DirectoryIterator::rememberEntry( EntryPtr pEntry ) {
    //fprintf( stderr, "remembering: %s\n", pEntry->name.c_str() );
	_entries.push_back( pEntry );
}

} //nsp my
