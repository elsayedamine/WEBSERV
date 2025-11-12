#ifndef SERVER_HPP
# define SERVER_HPP

#include <Configuration.hpp>
#include <sys/socket.h>
#include <set>
#include <main.hpp>

class Server 
{
    private:
        Configuration   config;
        // my variables
    public:
        Server() {}
        ~Server() {} // clear the project ig
        Server(const Configuration &);
        void    run( void ) const;
    private:
        // the main functions to run the server

};

#endif