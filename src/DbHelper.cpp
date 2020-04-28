#include "my/DbHelper.h"

namespace my {

DbHelper *DbHelper::_this = NULL;


DbHelper & DbHelper::instance() {
    if( ! _this ) {
        _this = new DbHelper();
        atexit( destroy );
    }
    return *_this;
}

StringPtr DbHelper::withTicks( const string &src ) {
    StringPtr pS( new string );
    pS->append( "'" );
    pS->append( src.c_str() );
    pS->append( "'" );
    return pS;
}

StringPtr DbHelper::toDbString( const string &src ) {
    char helpstr[2];
    helpstr[1] = 0;
    StringPtr pS( new string );
    const char* pSrc = src.c_str();
    pS->append( "'" );
    bool open = true;
    for( ; *pSrc; pSrc++ ) {
        if( *pSrc == QUOTE || *pSrc == TICK ) {
            if( open ) {
                pS->append( "' " );
                open = false;
            }
            pS->append( CONCAT );
            pS->append( *pSrc == QUOTE ? REPL_QUOTE : REPL_TICK );
        } else {
            if( !open ) {
                pS->append( CONCAT );
                pS->append( " '" );
                open = true;
            }
            helpstr[0] = *pSrc;
            pS->append( helpstr );
        }
    }
    if( open ) {
        pS->append( "'" );
    }
    return pS;
}

StringPtr DbHelper::toDbString( const MyDate &date, bool ISO ) {
    StringPtr pS( new string );
    if( !date.IsSet() ) {
        pS->append( "null" );
    } else {
        pS->append( "'" );
        pS->append( ISO ? date.ToIsoString() : date.ToEurString() );
        pS->append( "'" );
    }
    return pS;
}

DbHelper::DbHelper() {
    //ctor
}

void DbHelper::destroy() {
    if( _this ) {
        delete _this;
    }
}

DbHelper::~DbHelper() {
    //dtor
}

} //ns my
