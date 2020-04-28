
/* 
 * File:   server.h
 * Author: martin
 *
 * Created on 1. Januar 2020, 15:31
 */

#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <stdexcept>

namespace my {
    
static const int BUFFER_SIZE = 1024;

class ServerException : public std::runtime_error {
public:
    ServerException( const char* msg ) :
    std::runtime_error(msg)
    {
    }
};

class Server {
public:
    Server( int port );
    void create();
    void run();
    static void* workerthread_start_func( void* );
    virtual ~Server();
private: 
    void run_thread();
private:
    int _port;
    int _server_fd, _newsockfd;
    struct sockaddr_in _address;
    int _addrlen = sizeof(_address);
    
    const int MAX_READ = 1023;
    char _buffer[BUFFER_SIZE];
    bool _stop = false;
};


class ClientException : public std::runtime_error {
public:
    ClientException( const char* msg ) :
    std::runtime_error(msg)
    {
    }
};

class Client {
public:
    Client( int port );
    void create();
    void run();
private:
    int _port;
    int _socket;
    struct sockaddr_in _serv_addr;
    char _buffer[BUFFER_SIZE];
};

} //namespace my

#endif /* SERVER_H */

