
#include <my/ColumnInfo.h>

namespace my {

	/** Default-Ctor. */
	ColumnInfo::ColumnInfo() :
		_idx(-1), _name (""), _dataType( TYPE_UNKNOWN ),
		_size(-1), _decimalDigits(-1),	_isNullable( true)
	{}

	ColumnInfo::ColumnInfo( int idx,
		        const char * name,
				DataType dataType,
				unsigned long size,
				short decimalDigits,
				bool isNullable ) :
		_idx(idx),	_name(name), _dataType(dataType), _size(size),
		_decimalDigits(decimalDigits), _isNullable(isNullable)
	{}

	void ColumnInfo::setIndex( int idx ) {
		_idx = idx;
	}

	void ColumnInfo::setName( const char *name ) {
        _name.clear();
		_name.append( name );
	}

	void ColumnInfo::setDataType( DataType dtype ) {
		_dataType = dtype;
	}

	void ColumnInfo::setSize( unsigned long size ) {
		_size = size;
	}

	void ColumnInfo::setDecimalDigits( short nDecDigits ) {
		_decimalDigits = nDecDigits;
	}

	void ColumnInfo::setIsNullable( bool isNullable ) {
		_isNullable = isNullable;
	}

	int ColumnInfo::getIndex() const {
		return _idx;
	}

	const string & ColumnInfo::getName() const {
		return _name;
	}

	DataType ColumnInfo::getDataType() const {
		return _dataType;
	}

	unsigned long ColumnInfo::getSize() const {
		return _size;
	}

	short ColumnInfo::getDecimalDigits() const {
		return _decimalDigits;
	}

	bool ColumnInfo::isDecimal() const {
		return _dataType == TYPE_DECIMAL;
	}

	bool ColumnInfo::isInteger() const {
		return _dataType == TYPE_INT;
	}

	bool ColumnInfo::isShort() const {
		return _dataType == TYPE_SHORT;
	}

	bool ColumnInfo::isSmallInt() const {
		return isShort();
	}


	bool ColumnInfo::isFloat() const {
		return _dataType == TYPE_FLOAT;
	}

	bool ColumnInfo::isDouble() const {
		return _dataType == TYPE_DOUBLE;
	}


	bool ColumnInfo::isTime() const {
		return _dataType == TYPE_TIME;
	}

	bool ColumnInfo::isTimestamp() const {
		return _dataType == TYPE_TIMESTAMP;
	}

	bool ColumnInfo::isDate() const {
		return _dataType == TYPE_DATE;
	}

	bool ColumnInfo::isNullable() const {
		return _isNullable;
	}

	/**
		returns true for all kind of character,
		ansi, unicode, varchar.
		See IsAnsi..., IsUnicode... for details.
	*/
	bool ColumnInfo::isCharacter() const {
		return  ( _dataType == TYPE_CHAR ||
			      _dataType == TYPE_WCHAR ||
				  _dataType == TYPE_VARCHAR ||
				  _dataType == TYPE_WVARCHAR ||
				  _dataType == TYPE_NTS );
	}

	bool ColumnInfo::isVarchar() const {
		return  ( _dataType == TYPE_VARCHAR ||
				  _dataType == TYPE_WVARCHAR ||
				  _dataType == TYPE_NTS );
	}

	bool ColumnInfo::isAnsiCharacter() const {
		return _dataType == TYPE_CHAR;
	}

	bool ColumnInfo::isAnsiVarchar() const {
		return _dataType == TYPE_VARCHAR;
	}

	bool ColumnInfo::isUnicodeCharacter() const {
		return _dataType == TYPE_WCHAR;
	}

	bool ColumnInfo::isUnicodeVarchar() const {
		return _dataType == TYPE_WVARCHAR;
	}

	ColumnInfo::~ColumnInfo() {

	}

} // ns my
