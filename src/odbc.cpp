#ifdef WIN32
#include <my/odbc.h>
#include <my/CharBuffer.h>

#include <string>

namespace my {

	Odbc::Odbc() : _henv( NULL ), _hdbc( NULL ), _hstmt( NULL )
		     ,_varcharLimit( 1024 ) /*, _nullValueRepresentation( "" )*/
			 ,_connected( false )
			 ,_isCancelled( false )
	{}


	void Odbc::connect( const char *dsn, const char *user, const char *pwd ) {
		if( _hdbc && _connected ) {
			disconnect();
		}

		RETCODE rc = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &_henv);
		rc = SQLSetEnvAttr( _henv, SQL_ATTR_ODBC_VERSION,(SQLPOINTER) SQL_OV_ODBC3, SQL_IS_UINTEGER );
		checkRc( rc, SQL_HANDLE_ENV, _henv );
		rc = SQLAllocHandle(SQL_HANDLE_DBC, _henv, &_hdbc);
		checkRc( rc, SQL_HANDLE_DBC, _hdbc );

		rc = SQLConnect( _hdbc,
						(SQLTCHAR*)dsn ,
						SQL_NTS,
						(SQLTCHAR*)user,
						SQL_NTS,
						(SQLTCHAR*)pwd,
						SQL_NTS );


		checkRc( rc, SQL_HANDLE_DBC, _hdbc );
		/*rc = SQLAllocHandle( SQL_HANDLE_STMT, _hdbc, &_hstmt );
		CheckRc( rc );*/
		_connected = true;
		_dsn = dsn;
		_userId = ( user ) ? user : "" ;
		_pwd = ( pwd ) ? pwd : "";
	}

	void Odbc::reconnect() {
		connect( _dsn.c_str(), _userId.c_str(), _pwd.c_str() );
	}

	void Odbc::disconnect() {
		RETCODE rc = SQLDisconnect( _hdbc );
		//checkRc( rc, SQL_HANDLE_DBC, _hdbc );
		_hdbc = NULL;
		_connected = false;
	}

	/**
		sets the varchar limit to be retrieved via SQLGetData.
		Default = 1024.
		If the size of your varchar data is mainly > 1024 Byte you could
		speed up retrieval by increasing this limit.
	*/
	void Odbc::varcharLimit( int limit ) {
		_varcharLimit = limit;
	}

	/**
		gets the maximum count of characters to be retrieved
		from varchar columns
	*/
	int Odbc::varcharLimit() {
		return _varcharLimit;
	}

	DataTablePtr Odbc::select( const CharBuffer &sql ) {
		return select( string( sql.get() ) );
	}

	/**
	 * Führt ein SQL-Stmt aus und liefert das Ergebnis in
	 * einem DataTablePtr (SmartPointer) zurück.
	 * Wirft im Fehlerfall eine Exception.
	 * Der Aufrufer sollte sie auffangen und danach mit GetError die Detail-Informationen
	 * zum Fehler abholen.
	 * NB: Der Aufrufer muss sich um die Zerstörung des DataTable-Objekt NICHT kümmern!
	 */
	DataTablePtr Odbc::select( const string &sql ) {
		_isCancelled = false;
		RETCODE rc = SQLAllocHandle( SQL_HANDLE_STMT, _hdbc, &_hstmt );
		checkRc( rc, SQL_HANDLE_STMT, _hstmt );

#ifdef UNICODE
		SQLTCHAR *pSelect = Convert2Wchar( sql.c_str() );
#else
		SQLTCHAR *pSelect = (SQLTCHAR*)sql.c_str();
#endif
		rc = SQLPrepare( _hstmt, pSelect, SQL_NTS );
		checkRc( rc, SQL_HANDLE_STMT, _hstmt );
		rc = SQLExecute( _hstmt );
		checkRc( rc, SQL_HANDLE_STMT, _hstmt );

#ifdef UNICODE
		SysFreeString( pSelect );
#endif
		DataTablePtr spTbl ( new TableData() );
		provideColumnInfo( spTbl );
		fetch( spTbl );

		rc = SQLFreeHandle( SQL_HANDLE_STMT, _hstmt );
		checkRc( rc, SQL_HANDLE_STMT, _hstmt );

		return spTbl;
    }

	/** führt einen Select aus und überträgt das Ergebnis in spTbl.
	    Wirft im Fehlerfall eine MyOdbcException */
	void Odbc::select( const string &sql, DataTablePtr spTbl ) {
		_isCancelled = false;
		RETCODE rc = SQLAllocHandle( SQL_HANDLE_STMT, _hdbc, &_hstmt );
		checkRc( rc, SQL_HANDLE_STMT, _hstmt );

#ifdef UNICODE
		SQLTCHAR *pSelect = Convert2Wchar( sql.c_str() );
#else
		SQLTCHAR *pSelect = (SQLTCHAR*)sql.c_str();
#endif
		rc = SQLPrepare( _hstmt, pSelect, SQL_NTS );
		checkRc( rc, SQL_HANDLE_STMT, _hstmt );
		rc = SQLExecute( _hstmt );
		checkRc( rc, SQL_HANDLE_STMT, _hstmt );

#ifdef UNICODE
		SysFreeString( pSelect );
#endif
		if( spTbl->getColumnCount() > 0 ) {
			//wir sollen offenbar die neue Ergebnistabelle an die übergebene
		    //anhängen. Prüfen, ob das geht  (Anzahl Spalten und Format der Spalten
			//müssen übereinstimmen):
			DataTablePtr spCheck ( new TableData() );
			provideColumnInfo( spCheck );
			checkStruc( spTbl, spCheck );
		} else {
			provideColumnInfo( spTbl );
		}

		fetch( spTbl );

		rc = SQLFreeHandle( SQL_HANDLE_STMT, _hstmt );
		checkRc( rc, SQL_HANDLE_STMT, _hstmt );
	}

void Odbc::select( const string &sql, TableData &data ) {
	_isCancelled = false;
	RETCODE rc = SQLAllocHandle( SQL_HANDLE_STMT, _hdbc, &_hstmt );
	checkRc( rc, SQL_HANDLE_STMT, _hstmt );

#ifdef UNICODE
	SQLTCHAR *pSelect = Convert2Wchar( sql.c_str() );
#else
	SQLTCHAR *pSelect = (SQLTCHAR*)sql.c_str();
#endif
	rc = SQLPrepare( _hstmt, pSelect, SQL_NTS );
	checkRc( rc, SQL_HANDLE_STMT, _hstmt );
	rc = SQLExecute( _hstmt );
	checkRc( rc, SQL_HANDLE_STMT, _hstmt );

#ifdef UNICODE
	SysFreeString( pSelect );
#endif
	if( data.getColumnCount() > 0 ) {
		//wir sollen offenbar die neue Ergebnistabelle an die übergebene
		//anhängen. Prüfen, ob das geht  (Anzahl Spalten und Format der Spalten
		//müssen übereinstimmen):
		TableData check;
		provideColumnInfo( check );
		checkStruc( data, check );
	} else {
		provideColumnInfo( data );
	}

	fetch( data );

	rc = SQLFreeHandle( SQL_HANDLE_STMT, _hstmt );
	checkRc( rc, SQL_HANDLE_STMT, _hstmt );
}



	/** prüft, ob die beiden übergebenen Datentabellen die gleiche
	    Struktur haben (Anzahl + Art der Spalten).
		Der Spaltenname spielt keine Rolle.
		Wenn die Strukturen nicht übereinstimmen, wird eine
		MyOdbcException geworfen.*/
	void Odbc::checkStruc( DataTablePtr spData1, DataTablePtr spData2 ) {
		if( spData1->getColumnCount() != spData2->getColumnCount() ) {
			MyOdbcException ex(
				"Die Strukturen der Ergebnistabellen passen nicht zusammen: Anzahl Spalten unterschiedlich." );
			throw ex;
		}

		for( int c = 0, cmax = spData1->getColumnCount(); c < cmax; c++ ) {
			if( spData1->getColumnInfo( c ).getDataType() !=
				spData2->getColumnInfo( c ).getDataType() )
			{
				const char *pHeader1 = spData1->getColumnHeader( c );
				DataType type1 = spData1->getColumnInfo( c ).getDataType();
				const char *pTypename1 = DataTypeNames[type1];

				const char *pHeader2 = spData2->getColumnHeader( c );
				DataType type2 = spData2->getColumnInfo( c ).getDataType();
				const char *pTypename2 = DataTypeNames[type2];

				std::string msg( "Die Strukturen der Ergebnistabellen passen nicht zusammen.\n" );
				msg.append( "Folgende Spalten haben nicht die gleiche Art:\n" );
				msg.append( "Tabelle1/Spalte " );
				msg.append( pHeader1 );
				msg.append( " = " );
				msg.append( pTypename1 );
				msg.append( "\nTabelle2/Spalte " );
				msg.append( pHeader2 );
				msg.append( " = " );
				msg.append( pTypename2 );

				MyOdbcException ex( msg.c_str() );
				throw ex;
			}
		}
	}

void Odbc::checkStruc( const TableData &data1, const TableData &data2 ) const {
	if( data1.getColumnCount() != data2.getColumnCount() ) {
		MyOdbcException ex(
			"Die Strukturen der Ergebnistabellen passen nicht zusammen: Anzahl Spalten unterschiedlich." );
		throw ex;
	}

	for( int c = 0, cmax = data1.getColumnCount(); c < cmax; c++ ) {
		if( data1.getColumnInfo( c ).getDataType() !=
			data2.getColumnInfo( c ).getDataType() )
		{
			const char *pHeader1 = data1.getColumnHeader( c );
			DataType type1 = data1.getColumnInfo( c ).getDataType();
			const char *pTypename1 = DataTypeNames[type1];

			const char *pHeader2 = data2.getColumnHeader( c );
			DataType type2 = data2.getColumnInfo( c ).getDataType();
			const char *pTypename2 = DataTypeNames[type2];

			std::string msg( "Die Strukturen der Ergebnistabellen passen nicht zusammen.\n" );
			msg.append( "Folgende Spalten haben nicht die gleiche Art:\n" );
			msg.append( "Tabelle1/Spalte " );
			msg.append( pHeader1 );
			msg.append( " = " );
			msg.append( pTypename1 );
			msg.append( "\nTabelle2/Spalte " );
			msg.append( pHeader2 );
			msg.append( " = " );
			msg.append( pTypename2 );

			MyOdbcException ex( msg.c_str() );
			throw ex;
		}
	}
}

	/*
	 * [28.3.2009 15:12:22 testalucida]
	 *	Führt einen Update oder Insert aus.
	 */
	int Odbc::execute( const string &sql ) {
		SQLRETURN rc = SQLAllocHandle( SQL_HANDLE_STMT, _hdbc, &_hstmt );
		checkRc( rc, SQL_HANDLE_STMT, _hstmt );

#ifdef UNICODE
		SQLTCHAR *pExec = Convert2Wchar( sql.c_str() );
#else
		SQLTCHAR *pExec = (SQLTCHAR*)sql.c_str();
#endif
		rc = SQLPrepare( _hstmt, pExec, SQL_NTS );
		checkRc( rc, SQL_HANDLE_STMT, _hstmt );
		rc = SQLExecute( _hstmt );
		checkRc( rc, SQL_HANDLE_STMT, _hstmt );
		//int n = GetAffectedRows();
		SQLINTEGER rowCount;
		rc = SQLRowCount( _hstmt, &rowCount );

#ifdef UNICODE
		SysFreeString( pExec );
#endif
		rc = SQLFreeHandle( SQL_HANDLE_STMT, _hstmt );
		checkRc( rc, SQL_HANDLE_STMT, _hstmt );

		return rowCount;
	}

	/*
	 * [28.3.2009 18:07:54 testalucida]
	 *	Startet eine Transaction; damit wird der Auto-commit-mode ausgeschaltet.
	 * Nach Abarbeitung aller Updates der Transaktion muss Commit (oder Rollback)
	 * aufgerufen werden.
	 */
	void Odbc::startTransaction() {
		SQLRETURN rc = SQLSetConnectAttr( _hdbc, SQL_ATTR_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF, 0 );
		checkRc( rc, SQL_HANDLE_DBC, _hdbc );
	}

	/*
	 * [28.3.2009 18:10:40 testalucida]
	 *	Beendet eine Transaktion und schreibt alle �nderungen in die Datenbank.
	 */
	void Odbc::commit() {
		SQLRETURN rc = SQLEndTran( SQL_HANDLE_DBC, _hdbc, SQL_COMMIT );
		checkRc( rc, SQL_HANDLE_DBC, _hdbc );
		rc = SQLSetConnectAttr( _hdbc, SQL_ATTR_AUTOCOMMIT, (void*)1, 0 );
		checkRc( rc, SQL_HANDLE_DBC, _hdbc );
	}

	/*
	 * [28.3.2009 18:12:13 testalucida]
	 *	Beendet eine Transaktion und macht alle �nderungen r�ckg�ngig.
	 */
	void Odbc::rollback() {
		SQLRETURN rc = SQLEndTran( SQL_HANDLE_DBC, _hdbc, SQL_ROLLBACK );
		checkRc( rc, SQL_HANDLE_DBC, _hdbc );
		rc = SQLSetConnectAttr( _hdbc, SQL_ATTR_AUTOCOMMIT, (void*)1, 0 );
		checkRc( rc, SQL_HANDLE_DBC, _hdbc );
	}

	/** cancels the processing on a statement */
	void Odbc::cancelStatement() {
		_isCancelled = true;
	}

	Odbc::~Odbc() {
#ifdef UNICODE
		//... when done, free the BSTR
		::SysFreeString( _pDsn );
		::SysFreeString( _pUser );
		::SysFreeString( _pPwd );
#endif
	}


#ifdef UNICODE

	SQLTCHAR* Odbc::Convert2Wchar( const char *pAnsi ) {
		int a = lstrlenA( pAnsi );
		BSTR unicodestr = SysAllocStringLen(NULL, a);
		::MultiByteToWideChar(CP_ACP, 0, pAnsi, a, unicodestr, a );
		return unicodestr;
	}


	char* Odbc::Convert2MultiByte( const SQLTCHAR *pWchar ) {
		int a = lstrlenW( pWchar );
		char * multiByteStr = new char[1000];
		::WideCharToMultiByte( CP_ACP, 0, pWchar, -1, multiByteStr, 1000, NULL, NULL );
		return multiByteStr;
	}
#endif

	/*
	 * [28.3.2009 18:20:33 testalucida]
	 *	Prüft den übergebenen Returncode und wirft eine Exception,
	 * wenn dieser weder 0 noch 100 ist.
	 */
	void Odbc::checkRc( SQLRETURN rc, SQLSMALLINT handleType, SQLHANDLE handle ) const throw (...) {
		if( rc != 0 && rc != 100 ) {
			MyOdbcException ex( getError( handleType, handle ).c_str() );
			handle = NULL;
			throw ex;
		}
	}

	/**
	 * holt alle Ergebniszeilen des Selects und schreibt sie
	 * in die DataTable
	 */
	void Odbc::fetch( DataTablePtr & spTbl ) {
		SQLRETURN rc;
		//int numCols = spTbl->GetColumnCount();
		while( ( rc = SQLFetch( _hstmt ) ) != SQL_NO_DATA_FOUND ) {
			if( _isCancelled ) {
				SQLRETURN rc = SQLCancel( _hstmt ) ;
				checkRc( rc, SQL_HANDLE_STMT, _hstmt );
				spTbl->removeRows();
				return;
			}
			addRow( spTbl );
		}

		checkRc( SQLCloseCursor( _hstmt ), SQL_HANDLE_STMT, _hstmt );
	}

void Odbc::fetch( TableData &data ) {
	SQLRETURN rc;
	//int numCols = spTbl->GetColumnCount();
	while( ( rc = SQLFetch( _hstmt ) ) != SQL_NO_DATA_FOUND ) {
		if( _isCancelled ) {
			SQLRETURN rc = SQLCancel( _hstmt ) ;
			checkRc( rc, SQL_HANDLE_STMT, _hstmt );
			data.removeRows();
			return;
		}
		addRow( data );
	}

	checkRc( SQLCloseCursor( _hstmt ), SQL_HANDLE_STMT, _hstmt );
}


	/**
	 * stellt alle Spaltenwerte der aktuellen Row in ein DataRow-OBjekt,
	 * welches als SmartPointer zurückgegeben wird.
	 */
	void Odbc::addRow( DataTablePtr & rspTbl ) {
		SQL_DATE_STRUCT sqldate;
		SQL_TIME_STRUCT sqltime;
		SQL_TIMESTAMP_STRUCT sqltimestamp;

		int row = rspTbl->addRow();

		for( SQLUSMALLINT c = 0, max = rspTbl->getColumnCount(); c < max; c++ ) {
			const ColumnInfo &ci = rspTbl->getColumnInfo( c );
			int size = ci.getSize();

			int bytesRead = -1;

			if( ci.isCharacter() ) {
				string value;
				if( ci.isVarchar() ) {
					if( ( bytesRead = provideCellData( c+1, value ) ) >= 0 ) {
						rspTbl->setValue( value, row, c );
					}
				} else {
					char *pc = new char[size+1];
					pc[size] = '\0';
					if( ( bytesRead = provideCellData( c+1, pc, size+1 ) ) >= 0 ) {
						value.append( pc );
						rspTbl->setValue( value, row, c );
					}
					delete[] pc;
				}
			}  else if( ci.isInteger() ) {
				int i;
				if( ( bytesRead = provideCellData( c+1, &i, size ) ) >= 0 ) {
					rspTbl->setValue( i, row, c );
				}
			}  else if( ci.isTimestamp() ) {
				if( ( bytesRead = provideCellData( c+1, &sqltimestamp, size ) ) >= 0 ) {
					MyTimestamp ts( sqltimestamp.year, sqltimestamp.month, sqltimestamp.day,
									sqltimestamp.hour, sqltimestamp.minute, sqltimestamp.second, sqltimestamp.fraction );
					rspTbl->setValue( ts, row, c );
				}
			} else if( ci.isTime() ) {
				if( ( bytesRead = provideCellData( c+1, &sqltime, size ) ) >= 0 ) {
					MyTime time( sqltime.hour, sqltime.minute, sqltime.second, 0 );
					rspTbl->setValue( time, row, c );
				}
			}  else if( ci.isDate() ) {
				if( ( bytesRead = provideCellData( c+1, &sqldate, size ) ) >= 0 )  {
					MyDate date( sqldate.day, sqldate.month, sqldate.year );
					rspTbl->setValue( date, row, c );
				}
			}  else if( ci.isSmallInt() ) {
				short sh;
				if( ( bytesRead = provideCellData( c+1, &sh, size ) ) >= 0 ) {
					rspTbl->setValue( sh, row, c );
				}
			}  else if( ci.isFloat() ) {
				float fl;
				if( ( bytesRead = provideCellData( c+1, &fl, size ) ) >= 0 ) {
					rspTbl->setValue( fl, row, c );
				}
			} else if( ci.isDouble() ) {
				double dbl;
				if( ( bytesRead = provideCellData( c+1, &dbl, size ) ) >= 0 ) {
					rspTbl->setValue( dbl, row, c );
				}
			} else if( ci.isDecimal() ) {
				//TODO
			}
		}
	}

void Odbc::addRow( TableData & data ) {
	SQL_DATE_STRUCT sqldate;
	SQL_TIME_STRUCT sqltime;
	SQL_TIMESTAMP_STRUCT sqltimestamp;

	int row = data.addRow();

	for( SQLUSMALLINT c = 0, max = data.getColumnCount(); c < max; c++ ) {
		const ColumnInfo &ci = data.getColumnInfo( c );
		int size = ci.getSize();

		int bytesRead = -1;

		if( ci.isCharacter() ) {
			string value;
			if( ci.isVarchar() ) {
				if( ( bytesRead = provideCellData( c+1, value ) ) >= 0 ) {
					data.setValue( value, row, c );
				}
			} else {
				char *pc = new char[size+1];
				pc[size] = '\0';
				if( ( bytesRead = provideCellData( c+1, pc, size+1 ) ) >= 0 ) {
					value.append( pc );
					data.setValue( value, row, c );
				}
				delete[] pc;
			}
		}  else if( ci.isInteger() ) {
			int i;
			if( ( bytesRead = provideCellData( c+1, &i, size ) ) >= 0 ) {
				data.setValue( i, row, c );
			}
		}  else if( ci.isTimestamp() ) {
			if( ( bytesRead = provideCellData( c+1, &sqltimestamp, size ) ) >= 0 ) {
				MyTimestamp ts( sqltimestamp.year, sqltimestamp.month, sqltimestamp.day,
								sqltimestamp.hour, sqltimestamp.minute, sqltimestamp.second, sqltimestamp.fraction );
				data.setValue( ts, row, c );
			}
		} else if( ci.isTime() ) {
			if( ( bytesRead = provideCellData( c+1, &sqltime, size ) ) >= 0 ) {
				MyTime time( sqltime.hour, sqltime.minute, sqltime.second, 0 );
				data.setValue( time, row, c );
			}
		}  else if( ci.isDate() ) {
			if( ( bytesRead = provideCellData( c+1, &sqldate, size ) ) >= 0 )  {
				MyDate date( sqldate.day, sqldate.month, sqldate.year );
				data.setValue( date, row, c );
			}
		}  else if( ci.isSmallInt() ) {
			short sh;
			if( ( bytesRead = provideCellData( c+1, &sh, size ) ) >= 0 ) {
				data.setValue( sh, row, c );
			}
		}  else if( ci.isFloat() ) {
			float fl;
			if( ( bytesRead = provideCellData( c+1, &fl, size ) ) >= 0 ) {
				data.setValue( fl, row, c );
			}
		} else if( ci.isDouble() ) {
			double dbl;
			if( ( bytesRead = provideCellData( c+1, &dbl, size ) ) >= 0 ) {
				data.setValue( dbl, row, c );
			}
		} else if( ci.isDecimal() ) {
			//TODO
		}
	}
}

	/**
		read column value for all types except of varchar
	*/
	int Odbc::provideCellData( unsigned short colIdx, void *pBuffer, int bufferLen ) {
		long bytesRet = 0; //SQL_NTS;

		SQLRETURN rc = SQLGetData(	_hstmt			/*StmtHandle*/
									,colIdx			/*Col.Nr.*/
									,SQL_C_DEFAULT	/*TgtType*/
									,pBuffer		/*TgtValue*/
									,bufferLen		/*BufLen*/
									,&bytesRet		/*StrLen*/);
		checkRc( rc, SQL_HANDLE_STMT, _hstmt );
		return bytesRet;
	}

	/**
		read column value for varchar columns
	*/
	int Odbc::provideCellData( unsigned short colIdx, string & value ) {
		//char * pBuffer = new char[_varcharLimit+1];
		const int bufferLen = 2*_varcharLimit+1; //due to umlauts
		char * pBuffer = new char[bufferLen];

		long bytesRet = 0; //SQL_NTS;
		long totalRead = 0;

		SQLRETURN rc = 0;
		do {
			rc = SQLGetData( _hstmt			/*StmtHandle*/
							,colIdx			/*Col.Nr.*/
							,SQL_C_DEFAULT	/*TgtType*/
							,pBuffer 	    /*TgtValue*/
							,_varcharLimit	/*BufLen*/
							,&bytesRet		/*StrLen*/);
			if( ( rc == 0 || rc == SQL_SUCCESS_WITH_INFO ) && bytesRet > 0 ) {
				pBuffer[(bytesRet > _varcharLimit) ? _varcharLimit : bytesRet] = '\0';
//#ifdef _MBCS //multibyte character set
				//check for mutated vowels and replace them
				//replaceMutatedVowels( pBuffer, bufferLen );
//#endif

				value.append( pBuffer );
				totalRead += bytesRet;
			}
		} while( rc == SQL_SUCCESS_WITH_INFO );

		value.append( "\0" );

		delete[] pBuffer;

		checkRc( rc, SQL_HANDLE_STMT, _hstmt );

		return totalRead;
	}

	/**
	 * Methode liefert einen String mit den detaillierten Informationen,
	 * die das Datenbanksystem im Fehlerfall nach einem Select oder Update/Insert
	 * bereitstellt.
	 * Rufe diese Methode, wenn der Select/Update/Insert eine Exception geworfen hat.
	 */
	string Odbc::getError( SQLSMALLINT handleType, SQLHANDLE handle ) const {
		SQLTCHAR SQLState[6];
		SQLINTEGER NativeError;
		SQLTCHAR MessageText[1000];
		SQLSMALLINT BufferLength = 1000;
		SQLSMALLINT TextLength;

		SQLRETURN ret = SQLGetDiagRec( handleType, handle, 1,SQLState, &NativeError,
			MessageText, BufferLength,  &TextLength );

		if( ret == 0 ) {
			string msg;
			msg.append( "Fehler beim Datenbankzugriff!\n" );
			msg.append( "SQLState: " );
#ifdef UNICODE
			char * pC = Convert2MultiByte( SQLState );
			msg.append( pC );
			delete pC;
#else
			string s( (char*)SQLState );
			msg.append( s );
#endif
			msg.append( "\nNativeError: " );
			char buf10[10];
			sprintf_s( buf10, "%d\n", NativeError );
			msg.append( buf10 );
			msg.append( "\nFehlermeldung:\n" );
#ifdef UNICODE
			pC = Convert2MultiByte( MessageText );
			msg.append( pC );
			delete pC;
#else
			msg.append( (char*)MessageText );
#endif


			return msg;
		}
		return "";
	}

//#ifdef _MBCS
	//struct Replacement {
	//	char db2Umlaut;
	//	char msUmlaut;
	//};
	//static Replacement replacements[] = {
	//		 {-4, -68}  //ü
	//	   , {-10, -74} //ö
	//       , {-28, -92} //ä
	//	   , {-33, -97} //ß
	//	   , {-36, -100} //Ü
	//	   , {-39, -103} //Ù
	//	   , {-42, -106} //Ö
	//	   , {-45, -109} //Ó
	//	   , {-46, -110} //Ò
	//	   , {-55, -119} //É
	//	   , {-56, -120} //È
	//	   , {-60, -124} //Ä
	//	   , {0, 0}
	//};

	void Odbc::replaceMutatedVowels( char *pBuffer, int maxlen ) const {
		int len = strlen( pBuffer );
		for( char *p = pBuffer; *p; p++, len-- ) {
			//printf( "*p = %d\n", *p );
			if( *p < 0 ) { //mutated vowel
				char repl = *p - 64;
				//shift remaining string to the right
				memmove( p + 2, p + 1, len );
				len++;
				*p++ = -61;
				*p = repl;
			}
		}
	}

	//inline char Odbc::getMsUmlaut( char db2Umlaut ) const {
	//	for( Replacement *repl = replacements; repl->db2Umlaut != 0; repl++ ) {
	//		if( repl->db2Umlaut == db2Umlaut ) {
	//			return repl->msUmlaut;
	//		}
	//	}
	//	throw BadArgumentException( "Odbc::getMsUmlaut(): no such db2Umlaut" );
	//}
//#endif

	/**
	 * Ermittelt die Column-Infos des Ergebnisses eines Selects
	 * und schreibt sie in die DataTable
	 */
	void Odbc::provideColumnInfo( DataTablePtr & spTbl ) const {
		SQLSMALLINT numCols;
		SQLRETURN rc = SQLNumResultCols( _hstmt, &numCols );
		checkRc( rc, SQL_HANDLE_STMT, _hstmt );
		for( unsigned short c = 1; c <= numCols; c++ ) {
			spTbl->addColumn( getColumnInfo( c ) );
		}

	}

/**
	* Ermittelt die Column-Infos des Ergebnisses eines Selects
	* und schreibt sie in die DataTable
	*/
void Odbc::provideColumnInfo( TableData &data ) const {
	SQLSMALLINT numCols;
	SQLRETURN rc = SQLNumResultCols( _hstmt, &numCols );
	checkRc( rc, SQL_HANDLE_STMT, _hstmt );
	for( unsigned short c = 1; c <= numCols; c++ ) {
		data.addColumn( getColumnInfo( c ) );
	}

}

	ColumnInfoPtr Odbc::getColumnInfo( SQLUSMALLINT colIdx ) const {
		SQLTCHAR columnName[50]; //unsigned char
		SQLSMALLINT nameLen;
		SQLSMALLINT dataType;   //short
		SQLULEN columnSize;     //unsigned long
		SQLSMALLINT decimalDigits;
		SQLSMALLINT nullable;

		SQLRETURN rc = SQLDescribeCol( _hstmt, colIdx,
									   columnName, 50, &nameLen,
									   &dataType,
									   &columnSize,
									   &decimalDigits,
									   &nullable );
		checkRc( rc, SQL_HANDLE_STMT, _hstmt );

#ifdef UNICODE
		string sColName( Convert2MultiByte( columnName ) );
#else
		string sColName( (char *)columnName );
#endif

		/*ColumnInfo *pColInfo = */
		ColumnInfoPtr pColInfo( new ColumnInfo( colIdx - 1,
												sColName.c_str(),
												convertDataType( dataType ),
												columnSize,
												decimalDigits,
												nullable <= 0 ? false : true ) );

		return pColInfo;
	}

	DataType Odbc::convertDataType( SQLSMALLINT sqlDataType ) const {
		switch( sqlDataType ) {
			case SQL_CHAR:
			case SQL_DATA_AT_EXEC: //-2; e.g. IBMSNAP_INTENTSEQ (==char(10))
				return TYPE_CHAR;
			case SQL_VARCHAR:
			case SQL_LONGVARCHAR:
				return TYPE_VARCHAR;
			case SQL_WCHAR: //unicode char
				return TYPE_WCHAR;
			case SQL_WVARCHAR: //unicode varchar
			case SQL_WLONGVARCHAR: //dto.
				return TYPE_WVARCHAR;
			case SQL_NTS:
				return TYPE_NTS;
			case SQL_INTEGER:
				return TYPE_INT;
			case SQL_TYPE_DATE:
				return TYPE_DATE;
			case SQL_TYPE_TIMESTAMP:
				return TYPE_TIMESTAMP;
			case SQL_SMALLINT:
				return TYPE_SHORT;
			case SQL_TYPE_TIME:
				return TYPE_TIME;
			case SQL_FLOAT:
				return TYPE_FLOAT;
			case SQL_DOUBLE:
				return TYPE_DOUBLE;
			case SQL_DECIMAL:
				return TYPE_DECIMAL;
			default:
				string msg( "Odbc::ConvertDataType()\n\n" );
				msg.append( "Unknown SqlDataType: " );
				StringPtr sp = Convert::ToString( sqlDataType );
				msg.append( "\n" );
				msg.append( sp->c_str() );
				MyOdbcException ex( msg.c_str() );
				throw ex;
		}
	}
}

#endif
