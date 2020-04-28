
#ifndef _Odbc_H_
#define _Odbc_H_

#pragma warning( disable : 4239 ) //nonstandard extension i.Zsh. mit boost::any: Warnung unterdr�cken

#include "my/mystd.h"
#include <valarray>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>

#include <sql.h>
#include <sqlext.h>

#include <memory>
#include <string>

#include "my/odbcexception.hpp"
#include "my/ColumnInfo.h"
#include "my/convert.h"
#include "my/TableData.h"


/**
 * Wenn mit Unicode compiliert werden soll, muss UNICODE als Preprocessor-Define
 * eingetragen werden
 */

namespace my {

class CharBuffer;

class Odbc {
private:
	SQLHANDLE _henv, _hdbc, _hstmt;
	int _varcharLimit; //for multiple SQLGetData() calls
	std::string _dsn, _userId, _pwd;
public:
	Odbc();

	void connect( const char *dsn, const char *user, const char *pwd );
	void reconnect();
	const char *getDsn() const { return _dsn.c_str(); }
	const char *getUserId() const { return _userId.c_str(); }
	void disconnect();

	/**	sets the varchar limit to be retrieved via SQLGetData.
		Default = 1024.
		If the size of your varchar data is mainly > 1024 Byte you could
		speed up retrieval by increasing this limit. */
	void varcharLimit( int limit );

	/**	gets the maximum count of characters to be retrieved
		from varchar columns */
	int varcharLimit();

	/** F�hrt ein SQL-Stmt aus und liefert das Ergebnis in
	 * einem DataTablePtr (SmartPointer) zur�ck.
	 * Wirft im Fehlerfall eine Exception.
	 * Der Aufrufer sollte sie auffangen und danach mit GetError die Detail-Informationen
	 * zum Fehler abholen.
	 * NB: Der Aufrufer muss sich um die Zerst�rung des DataTable-Objekt NICHT k�mmern! */
	DataTablePtr select( const string &sql );
	DataTablePtr select( const CharBuffer &sql );

	/** F�hrt ein SQL-Stmt aus und liefert das Ergebnis in
	 * der übergebenen Datentabelle zurück.
	 * Wirft im Fehlerfall eine Exception.
	 * Der Aufrufer sollte sie auffangen und danach mit GetError die Detail-Informationen
	 * zum Fehler abholen.*/
	void select( const string &sql, DataTablePtr );
	void select( const string &sql, TableData & );

	/* [28.3.2009 15:12:22 testalucida]
	 *	F�hrt einen Update oder Insert aus.	 */
	int execute( const string &sql );

	/* [28.3.2009 18:07:54 testalucida]
	 *	Startet eine Transaction; damit wird der Auto-commit-mode ausgeschaltet.
	 * Nach Abarbeitung aller Updates der Transaktion muss Commit (oder Rollback)
	 * aufgerufen werden. */
	void startTransaction();

	/* [28.3.2009 18:10:40 testalucida]
	 *	Beendet eine Transaktion und schreibt alle �nderungen in die Datenbank. */
	void commit();

	/*[28.3.2009 18:12:13 testalucida]
	 *	Beendet eine Transaktion und macht alle �nderungen r�ckg�ngig. */
	void rollback();

	/** cancels the processing on a statement */
	void cancelStatement();

	~Odbc();

private:

#ifdef UNICODE

	SQLTCHAR* Convert2Wchar( const char *pAnsi );

	char *Convert2MultiByte( const SQLTCHAR *pWchar );
#endif

	/** prüft, ob die beiden übergebenen Datentabellen die gleiche
	    Struktur haben (Anzahl + Art der Spalten).
		Der Spaltenname spielt keine Rolle.
		Wenn die Strukturen nicht übereinstimmen, wird eine
		MyOdbcException geworfen.*/
	void checkStruc( DataTablePtr spData1, DataTablePtr spData2 );
	void checkStruc( const TableData &data1, const TableData &data2 ) const;

	/* [28.3.2009 18:20:33 testalucida]
	 *	Pr�ft den �bergebenen Returncode und wirft eine Exception,
	 * wenn dieser weder 0 noch 100 ist. */
	void checkRc( RETCODE rc, SQLSMALLINT handleType, SQLHANDLE handle ) const throw (...);

	/** holt alle Ergebniszeilen des Selects und schreibt sie
	 * in die DataTable */
	void fetch( DataTablePtr & spTbl );
	void fetch( TableData & );

	/** stellt alle Spaltenwerte der aktuellen Row in ein DataRow-OBjekt,
	 * welches als SmartPointer zur�ckgegeben wird. */
	void addRow( DataTablePtr & rspTbl );
	void addRow( TableData & );

	/** read column value for all types except of varchar*/
	int provideCellData( unsigned short colIdx, void *pBuffer, int bufferLen );

	/**	read column value for varchar columns*/
	int provideCellData( unsigned short colIdx, string & value );

	/** Methode liefert einen String mit den detaillierten Informationen,
	 * die das Datenbanksystem im Fehlerfall nach einem Select oder Update/Insert
	 * bereitstellt.
	 * Rufe diese Methode, wenn der Select/Update/Insert eine Exception geworfen hat. */
	string getError( SQLSMALLINT handleType, SQLHANDLE handle ) const;

	/** Ermittelt die Column-Infos des Ergebnisses eines Selects
	 * und schreibt sie in die DataTable */
	void provideColumnInfo( DataTablePtr & spTbl ) const;
	void provideColumnInfo( TableData &data ) const;

	ColumnInfoPtr getColumnInfo( SQLUSMALLINT colIdx ) const;

	DataType convertDataType( SQLSMALLINT sqlDataType ) const;
//#ifdef _MBCS
	void replaceMutatedVowels( char *pBuffer, int len ) const;
	//inline char getMsUmlaut( char db2Umlaut ) const;
//#endif

private:
	bool _connected;
	mutable bool _isCancelled;
};

}

#endif
