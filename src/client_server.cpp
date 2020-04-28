#include <my/client_server.h>

#include <unistd.h> 

#include <cstring>
#include <iostream>
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <pthread.h>

using namespace std;

namespace my {

    Server::Server( int port ) {
        _port = port;
    }

    void Server::run( ) {
        // Creating socket file descriptor 
        if( (_server_fd = socket( AF_INET, SOCK_STREAM, 0 )) == 0 ) {
            throw ServerException( "socket failed" );
        }

        // Forcefully attaching socket to the port 8080 
        int opt = 1;
        if( setsockopt( _server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                &opt, sizeof(opt) ) ) {
            throw ServerException( "setsockopt" );
        }
        _address.sin_family = AF_INET;
        _address.sin_addr.s_addr = INADDR_ANY;
        _address.sin_port = htons( _port );

        if( bind( _server_fd, (struct sockaddr *) &_address, sizeof(_address) ) < 0 ) {
            throw ServerException( "bind failed" );
        }
        if( listen( _server_fd, 3 ) < 0 ) {
            throw ServerException( "listen" );
        }

        while( _newsockfd = accept( _server_fd, 0, 0 ) ) {
            pthread_t t;
            pthread_create( &t, 0, Server::workerthread_start_func, (void*)this );
        }
    }

    void* Server::workerthread_start_func( void* pThis ) {
        char buf[1024];
        int count;
        pthread_detach( pthread_self( ) );
        Server* pServer = (Server*)pThis;
        int sockfd = pServer->_newsockfd;
        while( count = read( sockfd, buf, 1023 ) > 0 ) {
            cerr << "Server received: " << endl << buf << endl;
            cerr << "Server sending it back" << endl;
            write( sockfd, buf, strlen( buf ) );
        }

        close( sockfd );
    }

//    void Server::run( ) {
//        int new_socket;
//        int nRead = 0;
//        string messageFromClient;
//        while( !_stop ) {
//
//            cerr << "SERVER: now waiting for a connection" << endl;
//
//            if( (new_socket = accept( _server_fd, (struct sockaddr *) &_address,
//                    (socklen_t*) & _addrlen )) < 0 ) {
//                throw ServerException( "accept" );
//            }
//
//            cerr << "SERVER: accepted connection, now reading" << endl;
//
//            while( (nRead = read( new_socket, _buffer, MAX_READ )) > 0 ) {
//                _buffer[nRead] = 0x00;
//                messageFromClient += _buffer;
//                cerr << "SERVER: read provides: " << _buffer << endl;
//                if( nRead <= MAX_READ ) {
//                    break;
//                }
//            }
//            if( messageFromClient.length( ) > 0 ) {
//
//                cerr << "SERVER: reading completed: " <<
//                        messageFromClient.c_str( ) << endl;
//
//                if( messageFromClient.find( ":::CONV_END:::" ) != std::string::npos ) {
//                    _stop = true;
//                    cerr << "SERVER: got termination hint. Leaving loop." << endl;
//                }
//                messageFromClient.clear( );
//            }
//            cerr << "SERVER: closing socket" << endl;
//            close( new_socket );
//        }
//    }

    Server::~Server( ) {

    }

    //*************************************************************************

    Client::Client( int port ) {
        _port = port;
    }

    void Client::create( ) {
        if( (_socket = socket( AF_INET, SOCK_STREAM, 0 )) < 0 ) {
            throw ClientException( "\n Socket creation error \n" );
        }

        _serv_addr.sin_family = AF_INET;
        _serv_addr.sin_port = htons( _port );

        // Convert IPv4 and IPv6 addresses from text to binary form 
        if( inet_pton( AF_INET, "127.0.0.1", &_serv_addr.sin_addr ) <= 0 ) {
            throw ClientException( "\nInvalid address/ Address not supported \n" );
        }

        cerr << "CLIENT: connecting to socket" << endl;
        if( connect( _socket, (struct sockaddr *) &_serv_addr,
                sizeof(_serv_addr) ) < 0 ) {
            throw ClientException( "\nConnection Failed \n" );
        }
        cerr << "CLIENT: connect successfull, creation ended" << endl;

    }

    void Client::run( ) {
        //simulating some work:
        for( long n = 0, nmax = 800000; n < nmax; n++ ) {
            if( n % 100000 == 0 ) {
                snprintf( _buffer, BUFFER_SIZE, "found match: %ld\n", n );
                cerr << "CLIENT: now sending: " << _buffer << endl;
                send( _socket, _buffer, BUFFER_SIZE, 0 );
            }
        }
        //work is done:
        send( _socket, ":::CONV_END:::", ((string) ":::CONV_END:::").length( ), 0 );

    }

}//namespace my