#include <my/StringHelper.h>

#include <algorithm>

#ifndef WIN32
#include <string.h>
#endif

using namespace std;

namespace my {

StringHelper *StringHelper::_pThis = NULL;

StringHelper &StringHelper::instance() {
	if( !_pThis ) {
		_pThis = new StringHelper();
		atexit( destroy );
	}
	return *_pThis;
}

bool StringHelper::isEmpty(const char* pStr) const {
    for (const char* p = pStr; *p; p++) {
        if (*p > ' ') return false;
    }
    return true;
}

const char *StringHelper::getLesserIgnoreCase( const char *pVal1, const char *pVal2 ) {
	const char *p1=pVal1, *p2=pVal2;
	int c1, c2;
	for( ; *p1 && *p2; p1++, p2++ ) {
		c1 = tolower( *p1 );
		c2 = tolower( *p2 );
		if( c1 < c2 ) return pVal1;
		if( c1 > c2 ) return pVal2;
	}
	return ( *p1 ? pVal2 : pVal1 );
}

const char *StringHelper::getGreaterIgnoreCase( const char *pVal1, const char *pVal2 ) {
	const char *p1=pVal1, *p2=pVal2;
	int c1, c2;
	for( ; *p1 && *p2; p1++, p2++ ) {
		c1 = tolower( *p1 );
		c2 = tolower( *p2 );
		if( c1 < c2 ) return pVal2;
		if( c1 > c2 ) return pVal1;
	}
	return ( *p1 ? pVal1 : pVal2 );
}

void StringHelper::replaceAll( string &s, const string &search, const string &replace ) {
	for( size_t pos = 0; ; pos += replace.length() ) {
		// Locate the substring to replace
		pos = s.find( search, pos );
		if( pos == string::npos ) break;
		// Replace by erasing and inserting
		s.erase( pos, search.length() );
		s.insert( pos, replace );
	}
}

void StringHelper::replace( string &s, const string &search, const string &replace ) {
	size_t pos = 0;
	// Locate the substring to replace
	pos = s.find( search, pos );
	if( pos == string::npos ) return;
	// Replace by erasing and inserting
	s.erase( pos, search.length() );
	s.insert( pos, replace );
}

void StringHelper::replaceAll( string &s, const char search, const char replace ) {
	std::replace( s.begin(), s.end(), search, replace );
}

bool StringHelper::endsWith( const char *pString, const char *pCheck ) const {
    int chlen = strlen( pCheck );
    int slen = strlen( pString );
    if( chlen > slen ) return false;

    const char *pSub = pString + slen - chlen;
    return !strncmp( pSub, pCheck, chlen );

}

bool StringHelper::endsWith( const std::string &s, const char *pCheck ) const {
    int chlen = strlen( pCheck );
    int slen = s.length();
    if( chlen > slen ) return false;

    const char *pSub = s.c_str() + slen - chlen;
    return !strncmp( pSub, pCheck, chlen );

}

bool StringHelper::endsWithSlash( const std::string &s ) const {
	int len = s.length();
	if( len < 1 ) return false;
	char c = s.at( s.length() - 1 );
	return ( c == '\\' || c == '/' );
}

bool StringHelper::endsWithSlash( const char *pString ) const {
	char c = pString[strlen( pString ) - 1];
	return ( c == '\\' || c == '/' );
}

int StringHelper::compare( const char *pA, const char *pB, bool ignoreCase ) {
	char *pA2 = new char[strlen(pA)+1];
	char *pB2 = new char[strlen(pB)+1];
	if( ignoreCase ) {
		for( char *p = pA2; ( *p++ = tolower( *pA++ ) );  );
		for( char *p = pB2; ( *p++ = tolower( *pB++ ) );  );
	} else {
		strcpy( pA2, pA );
		strcpy( pB2, pB );
	}

	int rc = strcmp( pA2, pB2 );

	delete[] pA2;
	delete[] pB2;

	return rc;
}

char *StringHelper::copy( char *pDest, const char *pSrc, const char *pStart, const char *pEnd ) const {
	char *p = pDest;
	for( ; pStart <= pEnd && ( *p++ = *pStart++ ); );
	*p = '\0';
	return pDest;
}

void StringHelper::tokenize( const char *pSrc, char token, std::vector<string> &dest ) const {
	dest.clear();
	const char *p;
	string s;
        bool tokenFound = false;
	for( p = pSrc; *p; p++ ) {
		if( *p == token ) {
                    tokenFound = true;
			dest.push_back( s );
			s.clear();						
		} else {
			s.push_back( *p );
		}
	}
        if( tokenFound ) dest.push_back( s );
}

char *StringHelper::trim( char *pBuf ) const {
	while( *pBuf == ' ' ) {
		pBuf++;
	}

	if( !*pBuf ) return pBuf;

	char *p = pBuf + strlen( pBuf ) - 1;
	while( p > pBuf && *p == ' ' ) {
		p--;
	}
	*(p+1) = 0x00;
	return pBuf;
}

bool StringHelper::isNumeric( const char* cstr ) const {
    for( const char* p = cstr; *p; p++ ) {
        if( !( *p >= '0' and *p <= '9' ) ) {
            return false;
        }
    }
    return true;
}

void StringHelper::destroy() {
	delete _pThis;
}

} //nsp my
