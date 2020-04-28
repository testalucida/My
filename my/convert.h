#ifndef _CONVERT_H_
#define _CONVERT_H_


#include <sstream>

#include "my/mystd.h"
#include "my/compat.h"

using namespace std;

namespace my {

    class Convert {
    public:

		/** simple numeric checker: returns true when all
		chars represent digits. Even if a "." or "," is detected
		false will be returned. */
		static int isNumeric( const char *p ) {
			for( int i = 0, imax = strlen( p ); i < imax; i++, p++ ) {
				if( *p < 48 || *p > 57 ) {
					return false;
				}
			}
			return true;
		}

        /**
                Tries to convert a string into an Integer
         */
        static int ToInt(const string &strval) {
            return atoi(strval.c_str());
        }

        /** converts a c string to an integer */
        static int ToInt( const char *psz ) {
            return atoi( psz );
        }

        static float toFloat( const char *pVal ) {
            return (float)atof( pVal );
        }

        /* Converts an int value to null terminated c-string
         */
        static char* ToString(char* buf, int bufsize, int n) {
            Compat::intToCString(buf, bufsize, "%d", n);
            //sprintf( buf, bufsize, "%d", n );
            return buf;
        }

        static StringPtr toString( float f, const char *pFormat = "%.2f" ) {
            char buf[20];
            sprintf( buf, pFormat, f );
            StringPtr sp( new string( buf ) );
            return sp;
        }

        template < typename T > static StringPtr ToString(const T & value) {
            ostringstream ost;
            //ost.precision( 10 );
            ost.setf(ios::fixed,ios::floatfield);
            ost << value;
            StringPtr sp(new string(ost.str()));
            return sp;
        }

        template < typename T > static string& toString( const T & value, string &dest )  {
		   ostringstream ost;
		   //ost.precision( 10 );
		   ost.setf(ios::fixed,ios::floatfield);
		   ost << value;
		   dest.append( ost.str() );
		   return dest;
        }

        static string& toString( const string & value, string &dest ) {
        	dest = value;
        	return dest;
        }

		static void toUpper( string &str ) {
			char *pBuf = new char[str.size()+1];
			strcpy( pBuf, str.c_str() );
			toUpper( pBuf );
			str.clear();
			str.append( pBuf );

			delete[] pBuf;
		}

		static void toUpper( char *pCString ) {
			for( ; ( *pCString = toupper( *pCString ) ); pCString++ );
		}

    };

} //namespace my

#endif
