#ifndef _DBHELPER_H_
#define _DBHELPER_H_

#include "my/mystd.h"

#define QUOTE 34
#define TICK 39
#define REPL_QUOTE " chr(34) "
#define REPL_TICK " chr(39) "
#define CONCAT " & "

namespace my {

class DbHelper {
public:
    /** gets the one and only instance */
    static DbHelper &instance();
    /** surrounds a given string with ticks */
    StringPtr withTicks( const string &src );
    /** replaces all quotes and ticks by inserting a method call with
        the corresponding ASCII sign.
        surrounds the given string with ticks. */
    StringPtr toDbString( const string &src );
    /** returns an ISO or EUR string representation of the
        given date.
        In case of date being not set, "null" will be returned. */
    StringPtr toDbString( const MyDate &date, bool ISO );

    /** Default destructor */
    ~DbHelper();

private:
    /** Default constructor */
    DbHelper();
    static void destroy();
private:
    static DbHelper *_this;
};

} //ns my

#endif // _DBHELPER_H_
