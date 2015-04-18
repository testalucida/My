/*
 * MySql.h
 *
 *  Created on: 17.06.2012
 *      Author: max
 */

#ifndef MYSQLAPI_H_
#define MYSQLAPI_H_

#include <string>

using namespace std;

namespace my {
    class TableData;
}

class MySqlApiImpl;

/**
 *
 */
class MySqlApi {
public:
    MySqlApi();
	void connect( const char *user, const char *db, const char *passwd = 0,
			const char *host = "localhost" );
    void select( const char *pSql, my::TableData &data );
	void startTransaction();
	int exec( const char* sql );
	void commit();
	void rollback();
	const char* getErrorMessage() const;
	void disconnect();
	virtual ~MySqlApi();
private:
    MySqlApiImpl *_pImpl;
};

#endif /* MYSQL_H_ */
