#ifndef HEADER_E9F801211E92EBF4
#define HEADER_E9F801211E92EBF4

/*
 * TableData.h
 *
 *  Created on: 17.11.2012
 *      Author: max
 */

#ifndef TABLEDATA_H_
#define TABLEDATA_H_

#include <string>
#include <vector>
#include <memory>

#include <my/Signal.hpp>
#include <my/enums.h>
#include <my/mystd.h>
#include <my/ColumnInfo.h>


namespace my {

class TableData;
class SortTableData;
struct TableDataCallback;

class MyDate;
class MyTimestamp;
class MyTime;

struct DataChangedEvent {
	DataChangedEvent( TableDataChange, int, int );
	TableDataChange dataChange;
	int row;
	int col;
};

struct SortInfo {
	SortInfo() : isSorted(false), col(-1), sortDirection( SORTDIRECTION_NONE ) {}
	bool isSorted;
	int col;
	SortDirection sortDirection;
};

typedef std::shared_ptr<ColumnInfo> ColumnInfoPtr;
typedef std::vector<const char*> DataColumn;
typedef std::shared_ptr<DataColumn> DataColumnPtr;


/**
 * Class holds data to be used in a Flx_Table
 */
class TableData  {
public:
	Signal<TableData, DataChangedEvent> signalDataChanged;
public:
	TableData();
	virtual ~TableData();

	/** adds a column with database infos and returns
	 * the index of the new column */
	int addColumn( const ColumnInfo &ci );
	int addColumn( const ColumnInfoPtr pInfo );
	/** adds a column and returns the index of the new column
	 * Default data type is char */
	int addColumn( const char *pName, DataType type = TYPE_CHAR );
	/** copies all columns of the given TableData object without data */
	void copyColumns( const TableData & );
	/** adds a row and returns the index of that row */
	int addRow();
	/** adds count rows and returns the index of the last one */
	int addRows( int count );
	/** adds a Row and copies the contents of the specified row of the other
	    TableData */
	void copyRow( const TableData &, int row );
	const ColumnInfo &getColumnInfo( int col ) const;
	/** delete all rows */
	void removeRows();
	/** deletes one row */
	void removeRow( int row );
	/** deletes rows and columns */
	void clear();
	virtual int getColumnIndex( const char *pColumnName ) const;
	virtual int getColumnCount() const;
	virtual int getRowCount() const;
	virtual const char* getValue( int row, int col, const char *pDefault="" ) const;
	int getIntValue( int row, int col, int defaultVal=-1 ) const;
    float getFloatValue( int row, int col, float defaultVal = -1.0 ) const;
	MyDate getDateValue( int row, int col ) const;
	virtual void setValue( const char* pValue, int row, int col );
	virtual void setValue( const std::string &value, int row, int col );
	void setValue( int val, int row, int col );
	void setValue( double val, int row, int col );
	void setValue( float val, int row, int col );
	void setValue( const MyDate &val, int row, int col, bool iso = true );
	void setValue( const MyTimestamp &val, int row, int col, bool iso = true );
	void setValue( const MyTime &val, int row, int col );
	virtual const char* getColumnHeader( int col ) const;
    void setColumnHeader( int colIdx, const char * );
	virtual const char* getRowHeader( int row ) const;
	virtual void sort( int col, SortDirection sortdirection );
	const SortInfo &getSortInfo() const;
    /** saves content to csv-file.
	    pPathnfile needs not to have an extension */
	void saveAsCsv( const char *pPathnfile );
	/** checks if a given value can be found in one of the cells of
	    a given column. If so, the row number of the first find position
		will be returned. Searching starts with row startRow.
		Otherwise -1 will be returned. */
	int searchColumn( const char *pSearch, int col, int startRow = 0 ) const;
	/** checks if a given value can be found in one of the cells of
	    a given row. If so, the column number of the first find position
		will be returned. Searching starts with column startCol.
		Otherwise -1 will be returned. */
	int searchRow( const char *pSearch, int row, int startCol = 0 ) const;
	/** checks if a given value can be found in this table.
	    The first occurence will be returned in row, col. */
	void search( const char *pSearch, int &row, int &col ) const;
	void dump();
	void doCallback( bool callback );
	void doTableDataChangedCallback();
	void writeToFile( const char *pPathnFile ) const;
	void createFromFile( const char *pPathnFile );
private:
	int addColumn();
	void buildRowIndexes();
	void callCallbacks( TableDataChange chng, int row, int col );
	void deleteValues( DataColumnPtr );	
private:
	//this vector contains all data columns:
	std::vector<DataColumnPtr> _columns;
	//vector containing column infos ,
	//when containing data out of a database
	std::vector<ColumnInfoPtr> _columnInfos;
	//we need that one for sorting purposes:
	//in case of sorting we won't sort all rows
	//in the _columns vector but the indexes.
	//getValue and setValue will refer to the indexes
	//when delivering and setting values:
	std::vector<int> _indexes;

	SortInfo _sortInfo;
	bool _doCallback;
};

//================================================================

typedef shared_ptr<my::TableData> DataTablePtr;

//================================================================

class TableDataSort {
public:
    TableDataSort( DataColumnPtr pCol, const ColumnInfoPtr pCi, SortDirection sortdirection ) ;
    bool operator()( int idx1, int idx2 );
private:
    DataColumnPtr _pCol;
    bool _isChar;
	bool _isNumeric;
	bool _isDate;
	bool _isTimestamp;
	bool _isTime;
    SortDirection _sortdirection;
};

} /* namespace my */
#endif /* TABLEDATA_H_ */

#endif // header guard
