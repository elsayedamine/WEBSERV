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
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>


#define MAX_EVENTS 128

class Server 
{
	public:
		Configuration	config;
		std::map<int, Connection> connections;
		static int epoll_fd;

	public:
		Server() {}
		~Server(); // check clear the project ig && close fds
		Server(const Configuration &);
		void	run( void );
		static void	setEvents(int fd, int events, int mode);
	private:
		std::map<int, std::vector<ConfigBlock> > config_map;
		std::map<int, int> sockets_to_ports;
		std::map<int, int> client_fd_to_port;

		epoll_event	event;
		epoll_event	events[MAX_EVENTS];

		void		SetupSockets();
		int			acceptConnection(int listener_fd);
		void		closeConnection(int index);
		void		checkDuplicateServers(const ConfigBlock &new_server);
		void		handleConnectionIO(int index);
		void		handleWrite(int index);
		void		handleCGIIO(int fd);
		void		handleCGIWrite(Connection &connection, int &fd);
		void		handleCGIRead(Connection &connection, int &fd);
		pid_t		handleCGIExit(pid_t &cgi_pid, Response &response);
		void		cleanupCGI(Connection &con);
};

#endif