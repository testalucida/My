#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

#include <exception>
#include <string>

namespace my {

class BadArgumentException : public std::exception {
    public:
#ifdef WIN32

        BadArgumentException(const char * const what) : std::exception(what) {
        }
#else

        BadArgumentException(const char * const what) : std::exception() {
            _msg = what;
        }

        const char* what() const throw () {
            return _msg.c_str();
        }

        ~BadArgumentException() throw () {
        }
    private:
        std::string _msg;

#endif
    };

//-------------------------------------------------------
	
class IndexException : public std::exception {
    public:
#ifdef WIN32

        IndexException(const char * const what) : std::exception(what) {
        }

#else

        IndexException(const char * const what) : std::exception() {
            _msg = what;
        }

        const char* what() const throw () {
            return _msg.c_str();
        }

        ~IndexException() throw () {
        }
    private:
        std::string _msg;
#endif
    };

   


class InstanceException : public std::exception {
    public:
#ifdef WIN32

        InstanceException(const char * const what) : std::exception(what) {
        }
#else

        InstanceException(const char * const what) : std::exception() {
            _msg = what;
        }

        const char* what() const throw () {
            return _msg.c_str();
        }

        ~InstanceException() throw () {
        }
    private:
        std::string _msg;

#endif
};


class DataTableException : public std::exception {
    public:
#ifdef WIN32

        DataTableException(const char * const what) : std::exception(what) {
        }
#else

        DataTableException(const char * const what) : std::exception() {
            _msg = what;
        }

        const char* what() const throw () {
            return _msg.c_str();
        }

        ~DataTableException() throw () {
        }
    private:
        std::string _msg;

#endif
};

}

#endif