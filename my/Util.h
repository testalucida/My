/* 
 * File:   Util.h
 * Author: max
 *
 * Created on 21. Februar 2015, 10:13
 */

#ifndef UTIL_H
#define	UTIL_H

#include <my/CharBuffer.h>

namespace my {

class Util {
public:
    /**
     * Kind of a printf function but prints to stderr.
     * prints a new line with each call.
     * @param fmt: a format string analogue to printf
     * @param ... a variable argument list
     */
    static void log( const char* fmt, ... );

};

}

#endif	/* UTIL_H */

