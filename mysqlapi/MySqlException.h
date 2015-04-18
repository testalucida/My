/* 
 * File:   MySqlException.h
 * Author: max
 *
 * Created on 18. April 2015, 22:41
 */

#ifndef MYSQLEXCEPTION_H
#define	MYSQLEXCEPTION_H

#include <string>
#include <exception>

class MySqlException: public std::exception {
public:
	MySqlException( int err, const std::string &msg ) throw();

	/** Returns a C-style character string describing the general cause
	 *  of the current error.  */
	const char* what() const throw();
    
    virtual ~MySqlException();
    
private:
	int _errno;
	std::string _msg;
};

#endif	/* MYSQLEXCEPTION_H */

