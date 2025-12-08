#ifndef SERVER_HPP
# define SERVER_HPP

#include <Configuration.hpp>
#include <sys/socket.h>
#include <set>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/epoll.h>
#include <main.hpp>

class Server 
{
	public:
		Configuration   config;
		// my variables
	public:
		Server() {}
		~Server() {} // clear the project ig && close fds
		Server(const Configuration &);
		void    run( void );
	private:
		// the main functions to run the server
		std::map<int, std::vector<ConfigBlock> > config_map;
		std::map<int, int> sockets_to_ports;
		std::map<int, int> client_fd_to_port;
		// std::map<int, std::string> client_request_buffer;
		int epoll_fd;
		
		void	SetupSockets();
		int		accept_new_connection(int listener_fd);

};

#endif