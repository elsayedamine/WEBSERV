#include <Server.hpp>

int Server::epoll_fd = -1;
std::map<int, int> Server::pipe_to_client;

Server::Server(const Configuration & conf) : config(conf)
{
	const std::vector<ConfigBlock> &servers = config.getServers();

	for (std::size_t i = 0; i < servers.size(); ++i) {
		check_duplicate_servers(servers[i]);
		config_map[servers[i].port].push_back(servers[i]);
	}

	SetupSockets();

	if ((epoll_fd = epoll_create1(0)) < 0)
		throw std::runtime_error("epoll() failed.");
	std::map<int, int>::iterator it;
	for (it = sockets_to_ports.begin(); it != sockets_to_ports.end(); ++it) {
		struct epoll_event ep;
		ep.events = EPOLLIN;
		ep.data.fd = it->first;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, ep.data.fd, &ep) < 0)
 			throw std::runtime_error("epoll_ctl failed to add listener.");
	}
}

void	Server::check_duplicate_servers(const ConfigBlock &entering_server)
{
	int port = entering_server.port;
	std::string host = entering_server.host;

	if (config_map.find(port) != config_map.end())
	{
		std::vector<ConfigBlock> &existing_servers = config_map[port];
		for (size_t j = 0; j < existing_servers.size(); ++j)
		{
			if (existing_servers[j].host == host)
			{
				if (existing_servers[j].server_name == entering_server.server_name)
					throw std::runtime_error("Duplicate server block");

				for (size_t k = 0; k < entering_server.server_name.size(); ++k)
					for (size_t l = 0; l < existing_servers[j].server_name.size(); ++l)
						if (entering_server.server_name[k] == existing_servers[j].server_name[l])
							throw std::runtime_error("Conflict: server_name '" + entering_server.server_name[k] + "' already bound to this host/port");
			}
		}
	}
}

void	Server::SetupSockets()
{
	std::map<int, std::vector<ConfigBlock> >::iterator it;
	for (it = config_map.begin(); it != config_map.end(); ++it) {
		int port  = it->first;
		int sock;
		int opt = 1;
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			throw std::runtime_error("socket() failed.");

		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

		if (fcntl(sock, F_SETFL, O_NONBLOCK) < 0)
			throw std::runtime_error("fcntl() failed.");

		struct sockaddr_in addr;
		std::memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);

		if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
			throw std::runtime_error("bind() failed.");
		if (listen(sock, 1024) < 0)
			throw std::runtime_error("listen() failed.");

		this->sockets_to_ports[sock] = port;
		std::cout << "Server listening on port " << port << "..." << std::endl;
	}
}

// void handle_cgi_write(int pipe_fd)
// {
//     int client_fd = Server::pipe_to_client[pipe_fd];
//     // write request body to CGI stdin
//     ssize_t n = write(pipe_fd,
//                       Server::client_body[client_fd].data(),
//                       Server::client_body[client_fd].size());
//     if (n > 0)
//     {
//         // remove written part
//         Server::client_body[client_fd].erase(0, n);
//         if (Server::client_body[client_fd].empty())
//         {
//             close(pipe_fd);
//             epoll_ctl(Server::epoll_fd, EPOLL_CTL_DEL, pipe_fd, NULL);
//         }
//     }
// }

// string handle_cgi_read(int pipe_fd)
// {
// 	string output;
//     char buf[4096];
//     int client_fd = Server::pipe_to_client[pipe_fd];
//     ssize_t n = read(pipe_fd, buf, sizeof(buf));
//     if (n > 0)
//     {
//         Server::cgi_output[client_fd].append(buf, n);
//     }
//     else if (n == 0)
//     {
//         close(pipe_fd);
//         epoll_ctl(Server::epoll_fd, EPOLL_CTL_DEL, pipe_fd, NULL);
//         pid_t pid = Server::pipe_to_pid[pipe_fd];
//         waitpid(pid, NULL, WNOHANG);
//         // now CGI response is complete
//         // build_http_response(client_fd);
//     }
// }

int	Server::acceptConnection(int listener)
{
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client = accept(listener, (struct sockaddr *)&client_addr, &client_len);
	if (client < 0) { std::cerr << "Accept() failed" << std::endl; return false; }

	fcntl(client, F_SETFL, O_NONBLOCK);
	struct epoll_event client_event;
	client_event.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLOUT;
	client_event.data.fd = client;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client, &client_event) < 0) {
		std::cerr << "Failed to add client to epoll" << std::endl;
		close(client);
		return (false);
	}

	std::cout << "New connection on FD: " << client << std::endl;
	int port = sockets_to_ports[listener];
	this->client_fd_to_port[client] = port;
	return (true);
}

void Server::closeConnection(int index)
{
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[index].data.fd, NULL);
	connections.erase(events[index].data.fd);
	close(events[index].data.fd); // check again
}

void	Server::run()
{
	std::cout << "WebServer is running..." << std::endl;
	while (1)
	{
		int nevents = epoll_wait(epoll_fd, events, MAX_EVENTS, 3000);
		if (nevents < 0) continue;
		for (int i = 0; i < nevents; ++i)
		{
			int curr = events[i].data.fd;
			if (sockets_to_ports.find(curr) != sockets_to_ports.end())
				acceptConnection(curr);
			else if (events[i].events & (EPOLLERR | EPOLLRDHUP | EPOLLHUP))
				closeConnection(i);
			else if (pipe_to_client.count(curr))
				handleCGIIO(curr, events[i].events);
			else
				handleConnectionIO(curr, events[i].events);
		}
	}
	// close(epoll_fd);
	// std::cerr << "\nServers stopped\n";
}
