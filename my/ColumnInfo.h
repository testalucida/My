#ifndef COLUMNINFO_H_
#define COLUMNINFO_H_

#include <string>
using namespace std;

namespace my {

enum DataType {
	TYPE_CHAR,
	TYPE_WCHAR,
	TYPE_VARCHAR,
	TYPE_WVARCHAR,
	TYPE_INT,
	TYPE_SHORT,
	TYPE_DECIMAL,
	TYPE_FLOAT,
	TYPE_DOUBLE,
	TYPE_DATE,
	TYPE_TIME,
	TYPE_TIMESTAMP,
	TYPE_NTS, //null terminated string
	TYPE_UNKNOWN
};

static const char *DataTypeNames[] = {
	  "char" 
	, "wchar"
	, "varchar"
	, "wvarchar"
	, "int"
	, "short"
	, "decimal"
	, "float"
	, "double"
	, "date"
	, "time"
	, "timestamp"
	, "null terminated string"
	, NULL
};

class ColumnInfo {
private:
	int _idx;
	string _name;
	DataType _dataType;
	unsigned long _size;
	short _decimalDigits;
	bool _isNullable;
public:
	/** Default-Ctor. */
	ColumnInfo();

	/**
	* ctor.
	* idx: index der beschriebenen spalte, wie er aus dem DB-System geliefert wird
	* name: name der spalte
	* datatype: typ der spalte, SQL_xxx wie in sql.h oder sqlext.h definiert
	* size: groesse der spalte, sowohl bei num. wie auch bei char-spalten. bei decimal
	*       inkl. der nachkommastellen
	*		Achtung: Integer = 10, Float u. Double = 15, Short = 5
	* decimalDigits: anzahl der dezimalstellen, wenn datatype == TYPE_DECIMAL
	* isNullable: ob die spalte nullwerte enthalten darf
	*/
	ColumnInfo( int idx,
		        const char * name,
				DataType dataType,
				unsigned long size,
				short decimalDigits,
				bool isNullable );

	void setIndex( int idx );

	void setName( const char *name ) ;

	void setDataType( DataType dtype );

	void setSize( unsigned long size );

	void setDecimalDigits( short nDecDigits );

	void setIsNullable( bool isNullable );

	int getIndex() const;

	const string & getName() const;

	DataType getDataType() const;

	unsigned long getSize() const;
	
	short getDecimalDigits() const;

	bool isDecimal() const;

	bool isInteger() const;

	bool isShort() const;

	bool isSmallInt() const;


	bool isFloat() const;

	bool isDouble() const;


	bool isTime() const;

	bool isTimestamp() const;

	bool isDate() const;

	bool isNullable() const;

	/**
		returns true for all kind of character,
		ansi, unicode, varchar.
		See IsAnsi..., IsUnicode... for details.
	*/
	bool isCharacter() const;

	bool isVarchar() const;

	bool isAnsiCharacter() const;

	bool isAnsiVarchar() const;

	bool isUnicodeCharacter() const;

	bool isUnicodeVarchar() const;

	~ColumnInfo() ;

};
} // ns my

#endif