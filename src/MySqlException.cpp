
#include "../mysqlapi/MySqlException.h"

using namespace std;

MySqlException::MySqlException( int err, const string &msg ) throw() :
    _errno(err)
{
    char buf[15];
    sprintf( buf, "%d", err );
    _msg.append( "Error " );
    _msg.append( buf );
    _msg.append( ": " );
    _msg.append( msg );
}

MySqlException::~MySqlException() throw() {
}

/** Returns a C-style character string describing the general cause
 *  of the current error.  */
const char* MySqlException::what() const throw() {
    return _msg.c_str();
}


