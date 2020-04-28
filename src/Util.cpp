/* 
 * File:   Util.cpp
 * Author: max
 * 
 * Created on 21. Februar 2015, 10:13
 */

#include <my/Util.h>

#include <cstdarg>
#include <cstdio>

namespace my {

void Util::log( const char* fmt, ... ) {
    va_list list;
    va_start( list, fmt );
    
    for( const char *p = fmt; *p; p++ ) {
        if( *p != '%') {
            fprintf( stderr, "%c", *p );
            continue;
        }
        
        int i;
        double d;
        char *pStr;
        
        switch( *++p ) {
            case 'd':
                i = va_arg( list, int );
                fprintf( stderr, "%d", i );
                break;
            case 'f':
                d = va_arg( list, double );
                fprintf( stderr, "%f", d );
                break;
            case 's':
                pStr = va_arg( list, char* );
                if( pStr ) {
                    fprintf( stderr, "%s", pStr );
                } 
                break;
            default:
                putchar( *p );
                break;
        }
    }
    
    va_end( list );
    
    putchar( '\n' );
}


} //my