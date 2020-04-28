#ifndef _COMPAT_H_
#define _COMPAT_H_

#include <time.h>

#ifdef WIN32
#include <cstdio>
#else
#include <string.h>
#include <stdio.h>
#endif

namespace my {

class Compat {
public:
	inline static int intToCString( char *dest, int sizeofbuffer, const char *format, int n ) {
#ifdef WIN32
		return sprintf_s( dest, sizeofbuffer, format, n );
#else
		return sprintf( dest, format, n );
#endif
	}

	inline static char* strcpy( char *dest, int sizeofbuffer, const char *src ) {
#ifdef WIN32
		strcpy_s( dest, sizeofbuffer, src );
		return dest;
#else
		return ::strcpy( dest, src );
#endif
	}

	inline static char* strncpy( char *dest, int sizeofbuffer, const char *src, int cnt ) {
#ifdef WIN32
		strncpy_s( dest, sizeofbuffer, src, cnt );
		return dest;
#else
		return ::strncpy( dest, src, cnt );
#endif
	}

	inline static tm* localtime( tm *ptm, const time_t *pTime ) {
#ifdef WIN32
		localtime_s( ptm, pTime );
#else
        tm *ptmTmp = ::localtime( pTime );
        *ptm = *ptmTmp;
#endif
		return ptm;
	}
};

}

#endif
