#include <my/FileHelper.h>
#include <my/StringHelper.h>

#include <iostream>
#include <fstream>
#ifdef WIN32
#include <io.h>
#include <direct.h>
#define getCurDir _getcwd
#else
  //Linux
  #include <unistd.h>
  #include <sys/types.h>
  #include <sys/stat.h>
  #define getCurDir getcwd
  #include <string.h>
#endif

using namespace std;

namespace my {

void Lines::add( const string &line ) {
	_lines.push_back( line );
}

LinesIterator Lines::begin() {
	return _lines.begin();
}

LinesIterator Lines::end() {
	return _lines.end();
}

//////////////////////////////////////////////////////

FileHelper *FileHelper::_pThis = NULL;

FileHelper &FileHelper::instance() {
	if( !_pThis ) {
		_pThis = new FileHelper;
		atexit( destroy );
	}
	return *_pThis;
}

void FileHelper::write( const char *pContent, const char *pPathnFile ) {
	ofstream myfile;
	myfile.open( pPathnFile, ios::out );
	if( myfile.is_open() ) {
		myfile << pContent;
	} else {
		throw( "FileHelper::write():\nFile not open" );
	}
	myfile.close();
}

void FileHelper::writeLines( Lines &lines, const char *pPathnFile ) {
	ofstream myfile;
	myfile.open( pPathnFile, ios::out );
	if( myfile.is_open() ) {
		for( LinesIterator itr = lines.begin(); itr != lines.end(); itr++ ) {
			myfile << *itr << endl;
		}
	} else {
		throw( "FileHelper::write():\nFile not open" );
	}
	myfile.close();
}

StringPtr FileHelper::read( const char *pPathnFile ) {
	StringPtr pContent( new string );
	string line;
	ifstream myfile ( pPathnFile );

	if( myfile.is_open() ) {
		while ( myfile.good() )	{
			getline( myfile, line );
			pContent->append( line );
		}
		myfile.close();
		return pContent;
	} else {
		throw( "FileHelper::read():\nFile not open" );
	}
}

LinesPtr FileHelper::readLines( const char *pPathnFile ) {
	LinesPtr pLines( new Lines );
	ifstream myfile ( pPathnFile );

	if( myfile.is_open() ) {
		while ( myfile.good() )	{
			string line;
			getline( myfile, line );
			pLines->add( line );
		}
		myfile.close();
		return pLines;
	} else {
		throw( "FileHelper::readLines():\nFile not open" );
	}
}

void FileHelper::readLines( const char *pPathnFile, vector<string> & lines ) const {
	lines.clear();
	ifstream myfile ( pPathnFile );

	if( myfile.is_open() ) {
		while ( myfile.good() )	{
			string line;
			getline( myfile, line );

			//const char *pBuffer = line.c_str();
			//int len = strlen( pBuffer );
			//for( char *p = pBuffer; *p; p++, len-- ) {
			//	if( *p < 0 ) { //mutated vowel
			//		char repl = *p - 64;
			//		//shift remaining string to the right
			//		memmove( p + 2, p + 1, len );
			//		len++;
			//		*p++ = -61;
			//		*p = repl;
			//	}
			//}

			lines.push_back( line );
		}
		myfile.close();
		
	} else {
		throw( "FileHelper::readLines():\nFile not open" );
	}
}

PathAndFilePtr FileHelper::splitFileFromPath( const char *pPathnFile ) {
	PathAndFilePtr pfp( new PathAndFile );

	if( !strcmp( ".", pPathnFile ) ) {
		//current directory
		char buf[256];
		pfp->dir.append( getCurDir( buf, 256 ) );
    } else {
    #ifdef WIN32
		struct _finddata_t c_file;
		intptr_t hFile;
		if( ( hFile = _findfirst( pPathnFile, &c_file ) ) > -1L )  {
			if( c_file.attrib & _A_SUBDIR ) {
				//is directory; no splitting
				pfp->dir.append( pPathnFile );
			} else {
				//is file; splitting required
				char drive[10], dir[256], file[256], ext[20];
				_splitpath( pPathnFile, drive, dir, file, ext );
				pfp->drive.append( drive );
				pfp->dir.append( dir );
				pfp->file.append( file );
				pfp->file.append( ext );
				pfp->extension.append( ext );
			}
		} else { /*ignore*/ }
    #else
        throw ( "Linux not supported" );
    #endif
	}

	return pfp;
}

void FileHelper::splitLastSubDir( const char *pDir, std::string &dirRem, std::string &subDir ) const {
	const char *p = pDir + strlen( pDir ) - 1;
	bool removeLast = false;
	if( *p == '\\' || *p == '/' ) {
		p--;
		removeLast = true;
	}

	for( ; p > pDir && *p != '\\' && *p != '/'; p-- );

	if( p == pDir ) return; //no slash found

	char tmp[255];
	strcpy( tmp, p );
	if( removeLast ) tmp[strlen( tmp ) - 1] = '\0';
	subDir.append( tmp+1 );

	char *pTmp = tmp;
	for( const char *p2 = pDir; p2 < p; p2++ ) {
		*pTmp++ = *p2;
	}
	*pTmp = '\0';
	dirRem.append( tmp );
}

void FileHelper::splitDirectories( const char *pDir, std::vector<string> &dirs ) const {
	char part[255];
	for( const char *p = pDir, *pMax = pDir + strlen( pDir ); p < pMax; p++ ) {
		char c = *(p+1);
		if( c == '/' || c == '\\' || c == '\0' ) {
			string s( StringHelper::instance().copy( part, pDir, pDir, p ) );
			dirs.push_back( s );
		}
	}
}

bool FileHelper::existsDirectory( const char *pDirectory ) {
	bool exists = false;
	int n = strlen(pDirectory);
	char l = *(pDirectory + n - 1 );
	char *pDir = NULL;
	const char *pConstDir = pDirectory;
	if( l == '/' || l == '\\' ) {
		//directory; remove trailing (back-)slash for use with _findfirst
		pDir = new char[n];
		strncpy( pDir, pDirectory, n-1 );
		pDir[n-1] = '\0';
		pConstDir = pDir;
	}

#ifdef WIN32
	struct _finddata_t c_file;
	intptr_t hFile;
	if( ( hFile = _findfirst( pConstDir, &c_file ) ) > -1L )  {
		exists = ( c_file.attrib & _A_SUBDIR ) ? true : false;
	}
#else
    throw( "Linux not supported" );
#endif
	if( pDir ) delete[] pDir;
	return exists;
}

void FileHelper::ensureDirectory( const char *pDirectory, vector<string> *pCreatedDirs ) {
	char dir[255];
	char *p2 = dir;
	for( const char *p = pDirectory; ( *p2 = *p ); p++, p2++ ) {
		if( *p2 == '\\' || *p2 == '/' || *(p+1) == '\0' ) {
			*(p2+1) = '\0';
			if( !existsDirectory( dir ) ) {
                #ifdef WIN32
				_mkdir( dir );
				#else
				mkdir( dir, S_IRWXU );
				#endif
				if( pCreatedDirs ) {
					pCreatedDirs->push_back( dir );
				}
			}
		}
	}
}
void FileHelper::deleteFile( const char *pPathnFile ) {
	if( remove( pPathnFile ) != 0 ) {
		throw( "FileHelper::deleteFile: couldn't delete file.\n" );
	}
}

void FileHelper::deleteDirectory( const char *pPath ) {
	string cmd = "rmdir /S /Q ";
	cmd.append( pPath );
	system( cmd.c_str() );
}


void FileHelper::createDirectory( const char *pPath ) {
    #ifdef WIN32
	_mkdir( pPath );
	#else
	mkdir( pPath, S_IRWXU );
	#endif
}

void FileHelper::writeBinary( const char *pFilename, uchar *pData, long len ) const {
	ofstream textout( pFilename, ios::out | ios::binary);
	if( !textout.is_open() ) {
		throw( "FileHelper::writeBinary()\nCouldn't open File" );
	}
	try {
		textout.write( (const char*) pData, len );
	} catch(...) {
		throw( "FileHelper::writeBinary()\nFailed writing" );
	}

	textout.close();
}

BinaryData FileHelper::readBinary( const char *pFilename ) const {
	BinaryData bdata;
	const int blocksize = 512;
	bdata.pData = NULL;
	bdata.len = 0;

	ifstream textin( pFilename, ios::in | ios::binary );
	if( !textin.is_open() ) {
		throw( "FileHelper::readBinary()\nCouldn't open File" );
	}

	try {
		for( uchar *p = NULL; !textin.eof(); p += blocksize ) {
			bdata.pData = (uchar*)realloc( bdata.pData, blocksize *  sizeof( uchar ) );
			if( !bdata.pData ) {
				throw( "FileHelper::readBinary()\nCouldn't allocate buffer" );
			}
			if( ! p ) p = bdata.pData;
			textin.read( (char*)p, blocksize );
			bdata.len += (long)textin.gcount();
		}
	} catch(...) {
		throw( "FileHelper::readBinary()\nReading failed" );
	}

	textin.close();
	return bdata;
}

StringPtr FileHelper::getCurrentDirectory() const {
    char buf[256];
    getCurDir( buf, 256 );
    StringPtr p( new string( buf ) );
    return p;
}

void FileHelper::destroy() {
	delete _pThis;
}

} //nsp my
