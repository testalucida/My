#pragma once

#include <string>
#include <vector>

namespace my {

class StringHelper {
public:
	static StringHelper &instance();
	/**returns false if a char > ' ' is found else true */ 
	bool isEmpty(const char* pStr) const;
	/** replaces all occurencies of search in s by replace */
	void replaceAll( std::string &s, const std::string &search, const std::string &replace );
	/** replaces all occurencies of search in s by replace */
	void replaceAll( std::string &s, const char search, const char replace );
	/** replaces first occurency of search in s by replace */
	void replace( std::string &s, const std::string &search, const std::string &replace );
	/** checks if string s ends with c-string pCheck */
	bool endsWith( const std::string &s, const char *pCheck ) const;
	/** checks if c-string pString ends with c-string pCheck */
	bool endsWith( const char *pString, const char *pCheck ) const;
	/** checks if string s ends with slash or backslash. */
	bool endsWithSlash( const std::string &s ) const;
	/** checks if c-string pString ends with '/' or '\' */
	bool endsWithSlash( const char *pString ) const;
	/** returns the lesser one of the given c-strings. */
	const char *getLesserIgnoreCase( const char *pVal1, const char *pVal2 );
	/** returns the greater one of the given c-strings. */
	const char *getGreaterIgnoreCase( const char *pVal1, const char *pVal2 );
	/** compares 2 c-like strings and returns
	    0 if strings are equal
		-1  if a lt b
		1   if a gt b */
	int compare( const char *pA, const char *pB, bool ignoreCase = true );
	/** copies a given c-string (pSrc) to another c-string (pDest) 
	    beginning at position pStart and ending at position pEnd (both included)
		where pStart and pEnd must point to chars within  pSrc. */
	char *copy( char *pDest, const char *pSrc, const char *pStart, const char *pEnd ) const;
	/** zerlegt string pSrc anhand des Tokens token in Teilstrings, die in dest
	    gespeichert werden */
	void tokenize( const char *pSrc, char token, std::vector<std::string> &dest ) const;
	char *trim( char * ) const;
        bool isNumeric( const char* ) const;
private:
	StringHelper(){};
	static void destroy();
private:
	static StringHelper *_pThis;
};

} //nsp my
