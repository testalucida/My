#ifndef _TABLEDATASEARCH_
#define _TABLEDATASEARCH_

#include <my/Signal.hpp>

namespace my {

class TableData;

struct TableDataSearchHit {
	TableDataSearchHit() {
		row = 0;
		col = -1;
		pSubStr = (const char*)0;
	}
	int row;
	int col;
	const char *pSubStr;
};

struct SearchParm {
	SearchParm() 
		: searchForward( false )
		, pSearchString( (const char*)0 )
		, matchCase( false )
		, matchWord( false ) {}
	SearchParm( bool fwd, const char *pSearch, bool matchCase, bool matchWord )
		: searchForward( fwd )
		, pSearchString( pSearch )
		, matchCase( matchCase )
		, matchWord( matchWord ) {}
	bool searchForward;
	const char *pSearchString;
	bool matchCase;
	bool matchWord;
};

class TableDataSearch {
public:
	Signal< TableDataSearch, TableDataSearchHit > signalHit;
	Signal< TableDataSearch, TableDataSearchHit > signalSearchTerminated;
public:
	TableDataSearch( const TableData &data );
	void search( const SearchParm & );
private:
	TableDataSearchHit _lastHit;
	const TableData &_data;
};

}//my
#endif