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

#include <dirent.h>
#include <set>
#include <string>
#include <iostream>

class FDTracker {
private:
    std::set<int> known_fds; // FDs we expect to be open (listeners, standard FDs)

public:
    FDTracker() {
        // Initialize with standard FDs: 0,1,2 (stdin, stdout, stderr)
        known_fds.insert(0);
        known_fds.insert(1);
        known_fds.insert(2);
    }

    // Add expected FDs like listener sockets
    void addExpected(int fd) {
        known_fds.insert(fd);
    }

	void scanAndReport() {
		static std::set<int> reported; // remember already reported FDs
		std::set<int> current_fds;

		DIR *dir = opendir("/proc/self/fd/");
		if (!dir) return;
		struct dirent *entry;
		while ((entry = readdir(dir)) != NULL) {
			if (entry->d_name[0] == '.') continue;
			int fd = atoi(entry->d_name);
			current_fds.insert(fd);
		}
		closedir(dir);

		for (std::set<int>::iterator it = current_fds.begin(); it != current_fds.end(); ++it) {
			if (known_fds.find(*it) == known_fds.end() && reported.find(*it) == reported.end()) {
				std::cerr << "Potential leaked FD: " << *it << std::endl;
				reported.insert(*it);
			}
		}
}

};


#endif