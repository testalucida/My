#ifndef _CHARBUFFER_H_
#define _CHARBUFFER_H_

#include <memory> 
#include <vector>

namespace my {

struct ConvertChar { //codepage 1252 only
	char orig;
	char conv;
};

class CharBuffer;
typedef std::shared_ptr< CharBuffer > CharBufferPtr;
typedef std::vector< CharBufferPtr > CharBufferVector;
typedef std::shared_ptr< CharBufferVector > CharBufferVectorPtr;

static const int DEFAULT_SIZE = 25;

class CharBuffer {
public:
	/** Default Ctor. Initial size will be DEFAULT_SIZE */
	CharBuffer();
	/** Allocation of a CharBuffer to contain size characters */
	CharBuffer( int size );
	/** Allocation of a CharBuffer to contain pSrc as its initial value */
	CharBuffer( const char *pSrc );
	/** Copy Ctor */
	CharBuffer( const CharBuffer & );
	
	~CharBuffer();

	/** Add a character to this CharBuffer.
	    The CharBuffer's size will be adjusted automatically. */
	CharBuffer &add( char c );

	/** insert a char at the given position */
	void insert( char c, int pos );

	/** insert a c-string at the given position */
    void insert(const char *p, int pos );

	/** Add a c-like string to this CharBuffer.
	    The CharBuffer's size will be adjusted automatically. */
	CharBuffer &add( const char * );
    
    /** Add the c-string of another CharBuffer */
    CharBuffer &add( const CharBuffer & );

	/** Add an integer to this CharBuffer.
		The int-value will be converted to string according to
		the pFormat string */
	CharBuffer &addInt( int, const char *pFormat );
    
	/** Add an integer to this CharBuffer.
		The int-value will be converted to string using
		a default format string */
	CharBuffer &addInt( int );
    
    /** Add a float to this CharBuffer.
        The float-value will be converted to string according to
		the pFormat string  */
    CharBuffer &addFloat( float, const char *pFormat = "%.2f" );

	/** removes all trailing spaces */
	void rtrim();

	/** Converts all characters of this CharBuffer 
		to their lowercase equivalent if there is an 
		uppercase letter and has a lowercase equivalent.
		If no such conversion is possible, the concerned character 
		remains unchanged. */
	void toLower();

	/** Converts all characters of this CharBuffer 
		to their uppercase equivalent if there is an 
		lowercase letter and has an uppercase equivalent.
		If no such conversion is possible, the concerned character 
		remains unchanged. */
	void toUpper();

	/** removes the last n characters */
	void remove( int n );

	/** removes n characters beginning from offset */
	void remove( int offset, int n );

	/** replace all occurencies of substring pWhat by substring pBy */
	void replaceAll( const char *pWhat, const char *pBy );

	/** replace all occurencies of character what by substring pBy */
	void replaceAll( char what, const char *pBy );

	/** replace one occurency of pWhat by pBy starting search from start 
	    returning the offset of the first character of the substring 
		matching pWhat */
	int replace( const char *pWhat, const char *pBy, int start=0 );

	/** replace one occurency of what by pBy starting search from start 
	    returning the offset of the first character of the substring 
		matching what */
	int replace( char what, const char *pBy, int start=0 );

	/** splits this CharBuffer by any occurence of delim and returns all parts
	    in a CharBufferVectorPtr */
	CharBufferVectorPtr split( char delim ) const;

	/** removes all characters from this CharBuffer.
	    Resets its size to DEFAULT_SIZE (25)*/
	void clear();

	/** returns true if no character is assigned to this CharBuffer */
	bool isEmpty() const;

	/** Retrieve this CharBuffer's c-like string */
	const char *get() const;

	/** searches delimiter delim beginning at pStart searching to the left.
		Returns a pointer to the character right of delim.
		If delim cannot be found, the whole string is returned. */
	const char *getPartL( const char *pStart, char delim ) const;

	/** searches one of the given delimiters pDelims beginning at pStart 
	    searching to the left.
		Returns a pointer to the character right of the first found
		delimiter.
		If no delimiter cannot be found, the whole string is returned. 
		Each character within pDelims is expected to be a delimiter,
		no separation is needed. E.g.: " ,;-" contains the delimiters
		space, ",", ";", "-". */
	const char *getPartL( const char *pStart, const char *pDelims ) const;

	/** provides a substring of this CharBuffer into the given CharBuffer.
	    Returns a pointer to the other CharBuffer's c-string */
	const char *getSubstring( CharBuffer &, int start, int len ) const;

	/** returns a substring of this CharBuffer. */
	CharBufferPtr getSubstring( int start, int len ) const;

	/** gets the length of the string */
	int getLength() const;

	/** gets the size of the allocated buffer */
	int getSize() const;

	/** gets a pointer to the last character */
	const char *getLast() const;
    
    /** gets the number of the specified character*/
    int getNumberOf( char ) const;

	/** returns true if this CharBuffer's content is numeric */
	bool isNumeric() const;

	/** checks if pSubstring is part of this CharBuffer.
		Returns a pointer to pSubstring or NULl, 
		if pSubstring can not be found */
	const char *contains( const char *pSubstring ) const;

	/** searches for pWhat beginning at offset and returns a pointer
	    to the first occurence or NULL if no match can be found */
	const char *find( const char *pWhat, int offset = 0 ) const;

	/** searches for what beginning at offset and returns a pointer
	    to the first occurence of what or NULL if no match can be found */
	const char *find( char what, int offset = 0 ) const;

	/** searches for pWhat beginning at offset and returns the starting
		position of pWhat *related to offset* or -1 if pWhat cannot be found. */	    
	int find2( const char *pWhat, int offset = 0 ) const;

	/** compares the content of this CharBuffer with a given c-string.
	    NB: mutated vowels will be semantically compared regardless of
		the representation of these chars (1 or 2 byte).
		Returns 0 if strings are equal, 1 if this string is greater 
		(in alphabetic order) than pOther 
		and -1 if pOther is greater than this.*/
	int compareSmart( const char *pOther, bool ignoreCase=false ) const;

	/** provides information about the grade of congruence of 
		this CharBuffer with a given string. 
		Base is the longer one of both of the strings.
		E.g., if this == "Hans" and other == "Hansmann" the returned
		grade will be 5 (out of 10). 
		The comparison is "smart" (see compareSmart()) */
	int compareGrade( const char *pOther, bool ignoreCase=false ) const;

	/** return true if this CharBuffer is semantically equal
	    to pOther. (see compareSmart()) */
	bool isEqual( const char *pOther, bool ignoreCase=false ) const;
	bool isEqual( const CharBuffer &other, bool ignoreCase=false ) const;

	/** returns true if this string ends with the given substring */
	bool endsWith( const char *, bool ignoreCase=false ) const;

	/** Assignment operator */
	CharBuffer &operator=(const CharBuffer& other);
	CharBuffer &operator=(const char *);

	/** access CharBuffer by index */
	char operator[]( int i ) const;

	/** compare this CharBuffer's c-string with another 
	    CharBuffer's c-string. In contrast to compare(..) this is 
		not a semantic comparison but a byte oriented one.*/
	bool operator==(const CharBuffer &other) const;
	bool operator!=(const CharBuffer &other) const;
	bool operator>(const CharBuffer &other) const;

	/** compare this CharBuffer's c-string with another 
	    c-string. In contrast to compare(..) this is 
		not a semantic comparison but a byte oriented one.
		So "Müller" read from DB will not be recognized equal
		to "Müller" hard-coded. */
	bool operator==(const char *pOther) const;
	bool operator!=(const char *pOther) const;

	/** for testing purposes. prints contained characters */
	void dump();
private:
	void init( int size );
	void resize( int newsize );
private:
	char *_p;
	int _size; 
	int _strlen; 
};

} //my

#endif
