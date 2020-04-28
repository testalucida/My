#ifndef _ODBCEXCEPTION_HPP_
#define _ODBCEXCEPTION_HPP_

#include <string>
#include <stdexcept>

namespace my {

class MyOdbcException : public std::exception {
protected:
	std::string _message;
public:
	MyOdbcException( std::string message ) {
		_message = message;
	}

	virtual const char * what() {
		return _message.c_str();
	}

	virtual ~MyOdbcException(void) {

	}
};

}

#endif