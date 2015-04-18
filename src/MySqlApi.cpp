#include "../mysqlapi/MySqlApi.h"

#include "MySqlApiImpl.h"
#include <my/TableData.h>

MySqlApi::MySqlApi() //: _pImpl( new MySqlApiImpl() )
{
    _pImpl = new MySqlApiImpl();
}

void MySqlApi::connect( const char *user, const char *db, const char *passwd, const char *host ) 
{
    _pImpl->connect( user, db, passwd, host );
}

void MySqlApi::select( const char *pSql, my::TableData &data ) {
    _pImpl->select( pSql, data );
}

void MySqlApi::startTransaction() {
    _pImpl->startTransaction();
}

int MySqlApi::exec( const char* sql ) {
    return _pImpl->exec( sql );
}

void MySqlApi::commit() {
    _pImpl->commit();
}

void MySqlApi::rollback() {
    _pImpl->rollback();
}

const char* MySqlApi::getErrorMessage() const {
    return _pImpl->getErrorMessage();
}

void MySqlApi::disconnect() {
    _pImpl->disconnect();
}


MySqlApi::~MySqlApi() {
    delete _pImpl;
}


