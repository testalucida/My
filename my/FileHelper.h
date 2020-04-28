#pragma once

#include <my/mystd.h>

//static const char *CRLF = "\r\n"; //0x13 0x10

typedef unsigned char uchar;
typedef unsigned long ulong;

namespace my {

struct BinaryData {
	uchar *pData;
	ulong len;
};
//////////////////////////////////////////////////////
struct PathAndFile {
	std::string drive;
	std::string dir;
	std::string file; //with extension
	std::string extension;
};
//////////////////////////////////////////////////////
typedef std::vector<string>::iterator LinesIterator;

class Lines {
public:
	void add( const string &Line );
	LinesIterator begin();
	LinesIterator end();
	int getSize() { return _lines.size(); }
private:
	std::vector<string> _lines;
};
//////////////////////////////////////////////////////
typedef std::shared_ptr<PathAndFile> PathAndFilePtr;
//typedef std::vector<string*> Lines;
typedef std::shared_ptr<Lines> LinesPtr;

class FileHelper {
public:
	static FileHelper &instance();
	void write( const char *pContent, const char *pPathnFile );
	void writeLines( Lines &lines, const char *pPathnFile );
	/** returns the file's content in a string.
	    lines are separated by CRLF */
	StringPtr read( const char *pPathnFile );
	/** returns the file's content in a vector of strings.
	    each line is represented as a string in this vector */
	LinesPtr readLines( const char *pPathnFile );
	void readLines( const char *pPathnFile, vector<string> & lines ) const;
	PathAndFilePtr splitFileFromPath( const char *pPathnFile );
	/** splits the last (deepest) subdirectory from a given directory.
	    The remainder of the given directory will be provided in dirRem
		and the deepest subdirectory in subDir. There will be no trailing
		(back-)slashes neither in dirRem nor in subDir.
		pDir must not contain a trailing file. It would be considered to
		be the deepest subdirectory. */
	void splitLastSubDir( const char *pDir, std::string &dirRem, std::string &subDir ) const;
	/** splits the given directory into the directories contained.
	    E.g. C:\temp\test\junk results in 4 vector entries:
		C:
		C:\temp
		C:\temp\test
		C:\temp\test\junk */
	void splitDirectories( const char *pDir, std::vector<string> & ) const;
	/** checks if the given directory exists.
	    You may pass pDirectory with or w/o trailing (back-)slash */
	bool existsDirectory( const char *pDirectory );
	/** checks if the given directory exists and creates this
	    directory if it doesn't.
		May result in creation of more than one directory, depending
		on the number of not existing directories at the end of the given
		directory string.
		If pCreatedDirs is specified the created directories will be
		stored in there.*/
	void ensureDirectory( const char *pDirectory,
						  std::vector<std::string> *pCreatedDirs = NULL );
	void deleteFile( const char *pPathnFile );
	void deleteDirectory( const char *pPath );
	void createDirectory( const char *pPath );
	void writeBinary( const char *pFilename, uchar *pData, long len ) const;
	BinaryData readBinary( const char *pFilename ) const;
	StringPtr getCurrentDirectory() const;
private:
	FileHelper(){};
	static void destroy();
private:
	static FileHelper *_pThis;
};
//////////////////////////////////////////////////////

} //nsp my
