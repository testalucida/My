#include <my/TableDataSearch.h>
#include <my/TableData.h>
#include <my/convert.h>

namespace my {

TableDataSearch::TableDataSearch( const TableData &data ) 
	: _data( data )
{}

void TableDataSearch::search( const SearchParm &parm ) {
	short d = ( parm.searchForward ) ? 1 : -1;
	int c = _lastHit.col + d;
	int cmax = _data.getColumnCount();
	int r = _lastHit.row;
	if( c >= cmax ) {
		//SEARCH_FORWARD
		c = 0;
		r++;
	} else if( c < 0 ) {
		//SEARCH_BACKWARD
		c = cmax - 1;
		r = ( r > 0 ) ? --r : _data.getRowCount() - 1;
	}
	_lastHit.pSubStr = NULL;

	const char *pSearch;
	char *pBuf = NULL;
	if( !parm.matchCase ) {
		pBuf = new char[strlen( parm.pSearchString ) + 1];
		strcpy( pBuf, parm.pSearchString );
		Convert::toUpper( pBuf );
		pSearch = pBuf;
	} else {
		pSearch = parm.pSearchString;
	}
	for( int rmax = _data.getRowCount(); r < rmax  && r > -1; r += d ) {
		for( ; c < cmax && c > -1; c += d ) {
			const char *pValue = _data.getValue( r, c );
			char *pVal = NULL;
			if( !parm.matchCase ) {				
				pVal = new char[strlen( pValue )+1];
				strcpy( pVal, pValue );
				Convert::toUpper( pVal );
				pValue = pVal;
			} 
			if( parm.matchWord ) {
				//boah
			} else {
				const char* pSub = strstr( pValue, pSearch );
				if( pSub ) {
					//hit
					_lastHit.col = c;
					_lastHit.row = r;
					_lastHit.pSubStr = pSub;
					signalHit.send( this, &_lastHit );
					return;
				}
			}
			if( pVal ) delete[] pVal;
		}
		c = ( parm.searchForward ) ? 0 : cmax - 1;
	}

	if( r >= _data.getRowCount() || r < 0 ) {
		_lastHit.row = 0;
		_lastHit.col = -1;
		_lastHit.pSubStr = NULL;
		signalSearchTerminated.send( *this, _lastHit );
	}

	if( pBuf ) delete[] pBuf;
}

}//my