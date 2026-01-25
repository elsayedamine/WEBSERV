#ifndef SERVER_HPP
# define SERVER_HPP

#include <Configuration.hpp>
#include <Connection.hpp>

#include <sys/socket.h>
#include <set>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/epoll.h>
#include <wait.h>
#include <cstring>

#define MAX_EVENTS 128

class Server 
{
	public:
		Configuration	config;
		std::map<int, Connection> connections;
		static int epoll_fd;

		static std::map<int, int> pipe_to_client;
		// static int client_fd;
		// static std::map<int, pid_t> pipe_to_pid;
		// static std::map<int, std::string> cgi_responses;
		// std::map<int, size_t> pipe_write_offset;
	public:
		Server() {}
		~Server() {} // clear the project ig && close fds
		Server(const Configuration &);
		void	run( void );
	private:
		std::map<int, std::vector<ConfigBlock> > config_map;
		std::map<int, int> sockets_to_ports;
		std::map<int, int> client_fd_to_port;

		epoll_event	event;
		epoll_event	events[MAX_EVENTS];

		void	SetupSockets();
		int		acceptConnection(int listener_fd);
		void	closeConnection(int index);
		void	check_duplicate_servers(const ConfigBlock &new_server);
		void	handleConnectionIO(int fd, int events);
		void	handleCGIIO(int fd, int events);
};

#endif