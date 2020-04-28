#include <my/CharBuffer.h>

#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <locale>

namespace my {

CharBuffer::CharBuffer() 
	: _p( NULL )
	, _size( 0 )
	, _strlen( 0 )
{
	init( DEFAULT_SIZE );
}

CharBuffer::CharBuffer( int size ) 
	: _p( NULL )
	, _size( 0 )
	, _strlen( 0 )
{
	init( size );
}

CharBuffer::CharBuffer( const char *pSrc ) 
	: _p( NULL )
	, _size( 0 )
	, _strlen( 0 )
{
	if( pSrc ) {
		_strlen = strlen( pSrc );
		init( _strlen+1 );
		strcpy( _p, pSrc );
	} else {
		init( DEFAULT_SIZE );
	}
}

CharBuffer::CharBuffer( const CharBuffer &cb ) {
	_p = (char*)malloc( cb._size );
	assert( _p );
	_size = cb._size;
	strcpy( _p, cb._p );
	_strlen = cb._strlen;
}

void CharBuffer::init( int size ) {
	_p = (char*)malloc( size ); 
	assert( _p );
	_size = size;
	memset( _p, 0x000, _size );
}

void CharBuffer::resize( int size ) {
	char *pTmp = (char*)realloc( _p, size );
	assert( pTmp );
	_p = pTmp;
	_size = size;
}

CharBuffer &CharBuffer::add( char c ) {
	if( _strlen >= ( _size - 1 ) ) {
		//not enough space allocated.
		//reallocate using DEFAULT_SIZE.
		resize( _size + DEFAULT_SIZE );
	}
	_p[_strlen++] = c;
	_p[_strlen] = 0x00;
    
    return *this;
}

void CharBuffer::insert( char c, int pos ) {
	if( _strlen >= ( _size - 1 ) ) {
		//not enough space allocated.
		//reallocate using DEFAULT_SIZE.
		resize( _size + DEFAULT_SIZE );
	}
	memmove( _p+pos+1, _p+pos, _strlen-pos+1 ); //+1: move '\0' as well
	*(_p+pos) = c;
	_strlen++;
}

void CharBuffer::insert( const char *p, int pos ) {
	int len = strlen( p );
    if( _strlen + len >= ( _size - 1 ) ) {
        //not enough space allocated.
        //reallocate using DEFAULT_SIZE.
		int size = ( DEFAULT_SIZE > len ) ? DEFAULT_SIZE : len;
        resize( _size + size );
    }
    char *pDest = _p + pos + len;
    memmove( pDest, _p+pos, _strlen-pos+1 ); //+1: move '\0' as well
    memmove( _p+pos, p, len );
    _strlen += len;
}

CharBuffer &CharBuffer::add( const char *p ) {
	int len = strlen( p );
	if( (_size - _strlen) <= len ) {
		//not enough space allocated.
		//reallocate using DEFAULT_SIZE.
		int size = len < DEFAULT_SIZE ? DEFAULT_SIZE : (len+1);
		resize( _size + size );
	}
	strcat( _p, p );
	_strlen += len;
    
    return *this;
}

CharBuffer &CharBuffer::add(const CharBuffer& cb ) {
    add( cb.get() );
    return *this;
}

CharBuffer &CharBuffer::addInt( int n, const char *pFormat ) {
	char buf[20];
	sprintf( buf, pFormat, n );
	add( buf );
    
    return *this;
}

CharBuffer &CharBuffer::addInt( int n ) {
	char format[3];
	strcpy( format, "%d" );
	addInt( n, format );
    return *this;
}

CharBuffer &CharBuffer::addFloat( float f, const char *pFormat ) {
    char buf[20];
	sprintf( buf, pFormat, f );
	add( buf );
	return *this;
}

void CharBuffer::rtrim() {
	for( char *p = _p + (_strlen-1); (p >= _p) && (*p == ' '); p-- ) {
		*p = 0x00;
		_strlen--;
	}
}

void CharBuffer::toLower() {
	for( char *p = _p; *p; p++ ) {
		if( ( *p > 64 /*@*/ && *p < 91 /*[*/ ) /*A..Z*/ ||
			( *p != -61 && *p > -63 && *p < -33 ) /*SB encoded*/ ||
			( *p != -61 && *p > -129 && *p < -99 ) /*MB encoded*/
		  ) 
		{
			*p += 32;
		}
	}
}

void CharBuffer::toUpper() {
	for( char *p = _p; *p; p++ ) {
		if( ( *p > 96 /*`*/ && *p < 123 /*{*/ ) /*a..z*/ ||
			( *p > -33 && *p < 0 ) /*SB encoded*/ ||
			( *p != -61 && *p > -97 && *p < -65 ) /*MB encoded*/
		  )
		{
			*p -= 32;
		}
	}
}

void CharBuffer::remove( int n ) {
	for( char *p = _p + _strlen - 1; n > 0 && p >= _p; p--, n--, --_strlen ) {
		*p = 0x00;
	}
}

void CharBuffer::remove( int offset, int n ) {
	int offsetSrc = offset + n;
	int cnt = _strlen - offsetSrc;
	memmove( _p + offset, _p + offsetSrc, cnt );
	_strlen -= n;
	_p[_strlen] = 0x00;
}

void CharBuffer::replaceAll( const char *pWhat, const char *pBy ) {
    int offset = 0;
	for( const char *pRes = find( pWhat, offset ); pRes; pRes = find( pWhat, offset ) ) {
        if( pRes ) {
            offset = pRes - _p;
            remove( offset, strlen( pWhat ) );
            insert( pBy, offset );
            offset += strlen( pBy );
        }
    }
}

void CharBuffer::replaceAll( char what, const char *pBy ) {
    int offset = 0;
	for( const char *pRes = find( what, offset ); pRes; pRes = find( what, offset ) ) {
        if( pRes ) {
            offset = pRes - _p;
            remove( offset, 1 );
            insert( pBy, offset );
            offset += strlen( pBy );
        }
    }
}

int CharBuffer::replace( const char *pWhat, const char *pBy, int start ) {
	int offset = -1;
	const char *pRes = find( pWhat, start );
    if( pRes ) {
        offset = pRes - _p;
        remove( offset, strlen( pWhat ) );
        insert( pBy, offset );
    }
	return offset;
}

int CharBuffer::replace( char what, const char *pBy, int start ) {
	int offset = -1;
	const char *pRes = find( what, start );
    if( pRes ) {
        offset = pRes - _p;
        remove( offset, 1 );
        insert( pBy, offset );
    }
	return offset;
}

CharBufferVectorPtr CharBuffer::split( char delim ) const {
	CharBufferVectorPtr pBufVec( new CharBufferVector );
	const char *p = _p;
	CharBufferPtr pCb( new CharBuffer );
	for( ; *p; p++ ) {
		if( *p == delim ) {
			if( !pCb->isEmpty() ) {
				pBufVec->push_back( pCb );
			}
			CharBufferPtr pCbTmp( new CharBuffer );
			pCb = pCbTmp;			
		} else {
			pCb->add( *p );
		}
	}
	
	if( !pCb->isEmpty() ) {
		pBufVec->push_back( pCb );
	}

	return pBufVec;
}

void CharBuffer::clear() {
	resize( DEFAULT_SIZE );
	_p[0] = 0x00;
	_size = DEFAULT_SIZE;
	_strlen = 0;
}

bool CharBuffer::isEmpty() const {
	return ( _strlen == 0 );
}

const char *CharBuffer::get() const {
	return _p;
}

const char *CharBuffer::getPartL( const char *pStart, char delim ) const {
	const char *p;
	for( p = pStart; p >= _p; p-- ) {
		if( *p == delim ) {
			return ++p;
		}
	}
	return _p;
}

const char *CharBuffer::getPartL( const char *pStart, const char *pDelims ) const {
	const char *p;
	for( p = pStart; p >= _p; p-- ) {
		for( const char *pD = pDelims; *pD; pD++ ) {
			if( *p == *pD ) {
				return ++p;
			}
		}
	}
	return _p;
}

const char *CharBuffer::getSubstring( CharBuffer &buf, int start, int len ) const {
	for( const char *p = _p+start; *p && len > 0; p++, len-- ) {
		buf.add( *p );
	}
	return buf.get();
}

CharBufferPtr CharBuffer::getSubstring( int start, int len ) const {
	CharBufferPtr pBuf( new CharBuffer );
	for( const char *p = _p+start; *p && len > 0; p++, len-- ) {
		pBuf->add( *p );
	}
	return pBuf;
}

int CharBuffer::getLength() const {
	return _strlen;
}

int CharBuffer::getSize() const {
	return _size;
}

const char *CharBuffer::getLast() const {
	return _p + _strlen - 1;
}

int CharBuffer::getNumberOf( char c ) const {
    int cnt = 0;
    for( const char *p = _p; *p; p++ ) {
        if( *p == c ) cnt++;
    }
    return cnt;
}

bool CharBuffer::isNumeric() const {
	if( _strlen == 0 ) return false;

	for( const char *p = _p; *p; p++ ) {
		if( *p < 48 /*0*/ || *p > 57 /*9*/ ) return false;
	}
	return true;
}

const char *CharBuffer::contains( const char *pSubstring ) const {
	return strstr( _p, pSubstring );
}

const char *CharBuffer::find( const char *pWhat, int offset ) const {
	const char *p = _p + offset;
	return strstr( p, pWhat );
}

const char *CharBuffer::find( char what, int offset ) const {
	const char *p = _p + offset;
	return strchr( p, what );
}

int CharBuffer::find2( const char *pWhat, int offset ) const {
	const char *p = _p + offset;
	return ( strstr( p, pWhat ) - p );
}

int CharBuffer::compareSmart( const char *pOther, bool ignoreCase ) const {
	if( ignoreCase ) {
		CharBuffer cb1( _p );
		cb1.toLower();
		CharBuffer cb2( pOther );
		cb2.toLower();
		return cb1.compareSmart( cb2.get(), false );
	} 

	const char *p1 = _p;
	const char *p2 = pOther;

	for( ; *p1 && *p2; p1++, p2++ ) {
		if( *p1 == -61 ) {
			p1++;
		}
		if( *p2 == -61 ) {
			p2++;
		}
		if( *p1 == *p2 || 
			( *p1 < -67 && *p2 == (*p1+64) ) ||
			( *p2 < -67 && *p1 == (*p2+64) )
		  ) /* semantically equal */ continue; 
	
		if( *p1 > *p2 ) return 1;
		if( *p1 < *p2 ) return -1;
	}
	if( *p1 && !*p2 ) return 1;
	if( *p2 && !*p1 ) return -1;

	return 0;
}

int CharBuffer::compareGrade( const char *pOther, bool ignoreCase ) const {
	if( ignoreCase ) {
		CharBuffer cb1( _p );
		cb1.toLower();
		CharBuffer cb2( pOther );
		cb2.toLower();
		return cb1.compareGrade( cb2.get(), false );
	} 

	const char *p1 = _p;
	const char *p2 = pOther;

	int otherLen = strlen( pOther );
	int thisLen = _strlen;
	if( thisLen == 0 && otherLen == 0 ) return 0;

	int nMatch = 0;

	for( ; *p1 && *p2; p1++, p2++ ) {
		if( *p1 == -61 ) {
			p1++;
			thisLen--;
		}
		if( *p2 == -61 ) {
			p2++;
			otherLen--;
		}
		if( *p1 == *p2 || 
			( *p1 < -67 && *p2 == (*p1+64) ) ||
			( *p2 < -67 && *p1 == (*p2+64) )
		  ) 
		{
			/* semantically equal */ 
			nMatch++;		
			continue; 
		} else {
			break;
		}
	} //for

	int maxLen = thisLen > otherLen ? thisLen : otherLen;
	return (nMatch*10)/maxLen;
}

bool CharBuffer::isEqual( const char *pOther, bool ignoreCase ) const {
	return ( compareSmart( pOther, ignoreCase ) == 0 );
}

bool CharBuffer::isEqual( const CharBuffer &other, bool ignoreCase ) const {
	return ( compareSmart( other.get(), ignoreCase ) == 0 );
}

bool CharBuffer::endsWith( const char *pCheck, bool ignoreCase ) const {
	int len = strlen( pCheck );
	CharBuffer thisBuf = _p + _strlen - len;
	return ( thisBuf.compareSmart( pCheck, ignoreCase ) == 0 );
}

CharBuffer &CharBuffer::operator=(const CharBuffer& other) {
	if( this != &other ) {
		if( _p ) {
			free( _p );
		}
		_p = (char*)malloc( other._size );
		assert( _p );
		_size = other._size;
		strcpy( _p, other._p );
		_strlen = other._strlen;
	}
	return *this;
}

CharBuffer &CharBuffer::operator=(const char *s) {
	add( s );
	return *this;
}

char CharBuffer::operator[]( int i ) const {
	return *(_p + i);
}

bool CharBuffer::operator==(const CharBuffer& other) const {
	return ( strcmp( _p, other._p ) == 0 ) ? true : false;       
}

bool CharBuffer::operator!=(const CharBuffer& other) const {
	return ( strcmp( _p, other._p ) == 0 ) ? false : true;       
}

bool CharBuffer::operator>(const CharBuffer& other) const {
	 return ( compareSmart( other.get(), false ) == 1 ) ? true : false;
}

bool CharBuffer::operator==(const char * pOther) const {
	return ( strcmp( _p, pOther ) == 0 ) ? true : false;       
}

bool CharBuffer::operator!=(const char * pOther) const {
	return ( strcmp( _p, pOther ) == 0 ) ? false : true;       
}
 

void CharBuffer::dump() {
	printf( "--------------- D U M P ----------------\n" );
	printf( "printing c-like string %s\n", _p );
	for( const char *p = _p; *p; p++ ) {
		printf( "char: %d => %c\n", *p, *p );  
	}
	printf( "---------------------------------------\n" );
}

CharBuffer::~CharBuffer() {
	if( _p ) { //should always be
		free( _p );
	}
}

} //my
