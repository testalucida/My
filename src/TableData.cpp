/*
 * TableData.cpp
 *
 *  Created on: 17.11.2012
 *      Author: max
 */

#include <my/TableData.h>
#include <my/convert.h>
#include <my/datetime.h>
#include <my/FileHelper.h>
#include <my/StringHelper.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <cstdlib>

namespace my {

DataChangedEvent::DataChangedEvent( TableDataChange ch, int r, int  c )
	: dataChange( ch )
	, row( r )
	, col( c )
{}

TableData::TableData()
	: _doCallback( true )
{
}

TableData::~TableData() {
	for_each( _columns.begin(), _columns.end(), [&]( DataColumnPtr pDc ) {
        deleteValues( pDc );
	});
}

void TableData::deleteValues( DataColumnPtr pDc ) {
    for_each( pDc->begin(), pDc->end(), []( const char* pBuf ) {
        delete[] pBuf;
    });
}

int TableData::addColumn( const ColumnInfo &ci ) {
	int idx = addColumn();
	ColumnInfoPtr pCi( new ColumnInfo( ci ) );
	_columnInfos.push_back( pCi );
	callCallbacks( TABLEDATA_COLUMN_ADDED, -1, idx );
	return idx;
}

int TableData::addColumn( const ColumnInfoPtr pInfo ) {
	int idx = addColumn();
	_columnInfos.push_back( pInfo );
	callCallbacks( TABLEDATA_COLUMN_ADDED, -1, idx );
	return idx;
}

int TableData::addColumn( const char *pName, DataType type ) {
	int idx = addColumn();
	ColumnInfoPtr pCi( new ColumnInfo() );
	pCi->setName( pName );
	pCi->setDataType( type );
	_columnInfos.push_back( pCi );
	callCallbacks( TABLEDATA_COLUMN_ADDED, -1, idx );
	return idx;
}

int TableData::addColumn() {
	DataColumnPtr pCol( new DataColumn() );
	for( int r = 0, rmax = getRowCount(); r < rmax; r++ ) {
		pCol->push_back( NULL );
	}
	_columns.push_back( pCol );
	return _columns.size() - 1;
}

void TableData::copyColumns( const TableData &other ) {
	for( int c = 0, cmax = other.getColumnCount(); c < cmax; c++ ) {
		addColumn( other.getColumnInfo( c ) );
	}
}

void TableData::clear() {
	doCallback( false );
	//delete all cell values and build row indexes (empty array)
	removeRows();
	//remove all column entries
	_columns.clear();
	//...and all column information
	_columnInfos.clear();
	doCallback( true );
	callCallbacks( TABLEDATA_CLEARED, -1, -1 );
}

int TableData::addRow() {
	return addRows( 1 );
}

int TableData::addRows( int count ) {
    int rows = getRowCount();
	for_each( _columns.begin(), _columns.end(), [&]( DataColumnPtr pCol ) {
        for( int i = 0; i < count; i++ ) {
            pCol->push_back( NULL );
		}
	});

	for( int i = rows, imax = i+count; i < imax; i++ ) {
        _indexes.push_back( i );
        callCallbacks( TABLEDATA_ROW_ADDED, i, -1 );
	}
	return getRowCount() - 1;
}

void TableData::copyRow( const TableData &other, int row ) {
	int cmax = getColumnCount();
	if( cmax == 0 ) return;
	int r = addRow();
	for( int c = 0; c < cmax; c++ ) {
		const char *pVal = other.getValue( row, c );
		setValue( pVal, r, c );
	}
}

void TableData::removeRows() {
	for_each( _columns.begin(), _columns.end(), [&]( DataColumnPtr pCol ) {
        deleteValues( pCol );
		pCol->clear();
	});
	buildRowIndexes();
	callCallbacks( TABLEDATA_ALL_ROWS_REMOVED, -1, -1 );
}

void TableData::removeRow( int row ) {
	for_each( _columns.begin(), _columns.end(), [row]( DataColumnPtr pCol ) {
		vector<const char*>::iterator itr = pCol->begin();
		itr+=row;
		delete[] *itr;
		pCol->erase( itr );
	});
	buildRowIndexes();
	callCallbacks( TABLEDATA_ROW_REMOVED, row, -1 );
}

void TableData::buildRowIndexes() {
	_indexes.clear();
	int rows = getRowCount();
	for( int r = 0; r < rows; r++ ) {
        _indexes.push_back( r );
	}
	_sortInfo.isSorted = false;
	_sortInfo.col = -1;
	_sortInfo.sortDirection = SORTDIRECTION_NONE;
}

const ColumnInfo &TableData::getColumnInfo( int col ) const {
	return *_columnInfos[col];
}

int TableData::getColumnIndex( const char *pColumnName ) const {
	for( int c = 0, cmax = getColumnCount(); c < cmax; c++ ) {
		if( !strcmp( getColumnHeader( c ), pColumnName ) ) {
			return c;
		}
	}
	return -1;
}

int TableData::getColumnCount() const {
	return _columns.size();
}

int TableData::getRowCount() const {
	return ( _columns.empty() ? 0 : _columns[0]->size() );
}

const char* TableData::getValue( int row, int col, const char *pDefault ) const {
	DataColumnPtr pCol = _columns[col];
	const char *pVal = pCol->at( _indexes[row] );
	return ( pVal ? pVal : pDefault );
}

int TableData::getIntValue( int row, int col, int defaultVal ) const {
	const char *pVal = getValue( row, col, NULL );
	return ( pVal ? Convert::ToInt( pVal ) : defaultVal );
}

float TableData::getFloatValue( int row, int col, float defaultVal ) const {
    const char *pVal = getValue( row, col, NULL );
    if( pVal ) {
        return atof( pVal );
    }
    return defaultVal;
}

MyDate TableData::getDateValue( int row, int col ) const {
	MyDate dt;
	MyDate::FromString( dt, getValue( row, col ) );
	return dt;
}

void TableData::setValue( const char* pValue, int row, int col ) {
	char *pBuf;
    if( pValue ) {
        pBuf = new char[strlen(pValue)+1];
        strcpy( pBuf, pValue );
    } else {
        pBuf = new char;
        *pBuf = 0x0;
    }
	DataColumnPtr pCol = _columns[col];
	pCol->at( _indexes[row] ) = pBuf;
	callCallbacks( TABLEDATA_VALUE_CHANGED, row, col );
}

void TableData::setValue( const std::string& value, int row, int col ) {
	setValue( value.c_str(), row, col );
}

void TableData::setValue( int val, int row, int col ) {
	setValue( Convert::ToString( val )->c_str(), row, col );
}

void TableData::setValue( double val, int row, int col ) {
	setValue( Convert::ToString( val )->c_str(), row, col );
}

void TableData::setValue( float val, int row, int col ) {
	setValue( Convert::ToString( val )->c_str(), row, col );
}

void TableData::setValue( const MyDate &val, int row, int col, bool iso ) {
	setValue( iso ? val.ToIsoString().c_str() : val.ToEurString().c_str(), row, col );
}

void TableData::setValue( const MyTimestamp &val, int row, int col, bool iso ) {
	setValue( iso ? val.ToIsoString().c_str() : val.ToEurString().c_str(), row, col );
}

void TableData::setValue( const MyTime &val, int row, int col ) {
	setValue( val.ToString().c_str(), row, col );
}

//static char header[255];
const char* TableData::getColumnHeader( int col ) const {
	ColumnInfoPtr pCi = _columnInfos[col];
	//strcpy( header, pCi->getName().c_str() );
	//return header;
	return pCi->getName().c_str();
}

void TableData::setColumnHeader( int colIdx, const char *pName ) {
    ColumnInfoPtr pCi = _columnInfos[colIdx];
    pCi->setName( pName );
}

static char rowheader[255];
const char* TableData::getRowHeader( int row ) const {
    strcpy( rowheader, Convert::ToString( row )->c_str() );
	return rowheader;
}

void TableData::sort( int col, SortDirection sortdirection ) {
    TableDataSort datasort( _columns[col], _columnInfos[col], sortdirection );
    std::sort( _indexes.begin(), _indexes.end(), datasort );
	_sortInfo.isSorted = true;
	_sortInfo.col = col;
	_sortInfo.sortDirection = sortdirection;
    //fprintf( stderr, "TableData::sort()\n" );
    callCallbacks( TABLEDATA_SORTED, -1, -1 );
}

const SortInfo &TableData::getSortInfo() const {
	return _sortInfo;
}

void TableData::doCallback( bool callback ) {
	_doCallback = callback;
}
void TableData::callCallbacks( TableDataChange chng, int row, int col ) {
	if( _doCallback ) {
		DataChangedEvent evt( chng, row, col );
		signalDataChanged.send( this, &evt );
	}
}

void TableData::doTableDataChangedCallback() {
	callCallbacks( TABLEDATA_CHANGED, -1, -1 );
}

void TableData::saveAsCsv( const char *pPathnfile ) {
	string values; //Spaltenwerte der ganzen Tabelle
	//erst die Header:
	for( int c = 0, cmax = getColumnCount(); c < cmax; c++ ) {
		values.append( getColumnHeader( c ) );
		if( c < cmax - 1 ) {
			values.append( ";" );
		}
	}

	values.append( "\n" );

	for( int r = 0, rmax = getRowCount(); r < rmax; r++ ) {
		for( int c = 0, cmax = getColumnCount(); c < cmax; c++ ) {
			values.append( getValue( r, c, "<null>" ) );
			if( c < cmax-1 ) {
				values.append( ";" );
			}
		}
		values.append( "\n" );
	}
	FileHelper::instance().write( values.c_str(), pPathnfile );
}

int TableData::searchColumn( const char *pSearch, int col, int startRow ) const {
	//char *pSearchLow = new char[strlen(pSearch)+1];
	int rowFound = -1;
	StringHelper &sh = StringHelper::instance();
	for( int r = startRow, rmax = getRowCount(); r < rmax; r++ ) {
		const char *pVal = getValue( r, col );
		if( !sh.compare( pVal, pSearch ) ) {
			rowFound = r;
			break;
		}
	}
	//delete[] pSearchLow;

	return rowFound;
}

int TableData::searchRow( const char *pSearch, int row, int startCol ) const {
	throw( "not yet implemented" );
}

void TableData::search( const char *pSearch, int &row, int &col ) const {
	for( int c = 0, cmax = getColumnCount(); c < cmax; c++ ) {
		row = searchColumn( pSearch, c, 0 );
		if( row > -1 ) {
			col = c;
			return;
		}
	}
	row = -1;
	col = -1;
}

void TableData::dump() {
    for( int r = 0, rmax = getRowCount(); r < rmax; r++ ) {
        std::cout << "Value at ";
        for( int c = 0, cmax = getColumnCount(); c < cmax; c++ ) {
            std::cout << r << "/" << c << ": " << getValue( r, c ) << "\t";
        }
        std::cout << endl;
    }
}

//==============================================================

TableDataSort::TableDataSort( DataColumnPtr pCol, const ColumnInfoPtr pCi, SortDirection sortdirection ) 
	: _pCol( pCol )
	, _isChar( pCi->isCharacter() )
	, _isNumeric( false )
	, _isDate( false )
	, _isTimestamp( false )
	, _isTime( false )
	, _sortdirection( sortdirection )
{
	if( !_isChar ) {
		_isDate = pCi->isDate();
		if( !_isDate ) {
			_isTimestamp = pCi->isTimestamp();
			if( !_isTimestamp ) {
				_isTime = pCi->isTime();
				if( !_isTime ) {
					_isNumeric = true;
				}
			}
		}
	}
	
	//printf( "TABLEDATASORT: %s\n", _sortdirection == SORTDIRECTION_ASC ? "aufsteigend" : "absteigend" );
}

bool TableDataSort::operator()( int idx1, int idx2 ) {
    const char *pVal1 = _pCol->at( idx1 );
    const char *pVal2 = _pCol->at( idx2 );
    if( !pVal1 ) {
        return false;
    }
    if( !pVal2 ) {
        return true;
    }

    if ( _isChar ) {
        return ( _sortdirection == SORTDIRECTION_ASC ?
                 strcmp( pVal1, pVal2 ) < 0 :
                 strcmp( pVal1, pVal2 ) > 0 );
	

    } else if( _isNumeric ) {
        // Numeric sort
        int a = 0;
        sscanf( pVal1, "%d", &a );
        int b = 0;
        sscanf( pVal2, "%d", &b );
        return( _sortdirection == SORTDIRECTION_ASC ? a < b : b < a );
	} else if( _isDate ) {
		MyDate d1;
		MyDate::FromString( d1, pVal1 );
		MyDate d2;
		MyDate::FromString( d2, pVal2 );
		return( _sortdirection == SORTDIRECTION_ASC ) ? d1 < d2 : d2 < d1;
    } else if( _isTimestamp ) {
		MyTimestampPtr ts1 = MyTimestamp::FromIsoString( pVal1 );
		MyTimestampPtr ts2 = MyTimestamp::FromIsoString( pVal2 );
		return( _sortdirection == SORTDIRECTION_ASC ) ? *ts1 < *ts2 : *ts2 < *ts1;
	} else { //_isTime
		MyTimePtr t1 = MyTime::FromString( pVal1 );
		MyTimePtr t2 = MyTime::FromString( pVal2 );
		return( _sortdirection == SORTDIRECTION_ASC ) ? *t1 < *t2 : *t2 < *t1;
	}
}

void TableData::writeToFile( const char *pPathnFile ) const {
	string content;
	bool containsSemic = false;
	char buf[10];
	for( int r = -1, rmax = getRowCount(), cmax = getColumnCount(); r < rmax; r++ ) {
		for( int c = 0; c < cmax; c++ ) {
			containsSemic = false;
			const char *pVal;
			if( r == -1 ) {
				pVal = getColumnHeader( c );
				if( !pVal ||  strlen( pVal ) == 0 ) {
					StringPtr ptr = Convert::ToString( c );
					strcpy( buf, ptr->c_str() );
					pVal = buf;
				}
			} else {
				pVal = getValue( r, c );
			}
			
			if( strchr( pVal, ';' ) ) {
				containsSemic = true;
			}
			if( containsSemic ) {
				content.append( "\"" );
			}
			content.append( pVal );
			if( containsSemic ) {
				content.append( "\"" );
			}
			content.append( ";" );
		}
		content.append( "\n" );
	}

	FileHelper::instance().write( content.c_str(), pPathnFile );
}

void TableData::createFromFile( const char *pPathnFile ) {
	vector< string > lines;
	FileHelper::instance().readLines( pPathnFile, lines );
	int n = 0;
	for( vector< string >::iterator itr = lines.begin(); itr != lines.end(); itr++, n++ ) {
		string &line = *itr;
		if( n == 0 ) {
			//first line containing column headers
		} else {
		}
	}

}

} /* namespace my */
