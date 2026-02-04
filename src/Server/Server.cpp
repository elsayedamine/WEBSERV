#include <Server.hpp>

int serverRunning = 1;
int Server::epoll_fd = -1;

Server::Server(const Configuration & conf) : config(conf)
{
	const std::vector<ConfigBlock> &servers = config.getServers();

	for (std::size_t i = 0; i < servers.size(); ++i) {
		checkDuplicateServers(servers[i]);
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

void Server::checkDuplicateServers(const ConfigBlock &entering_server)
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

void Server::SetupSockets()
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
		addr.sin_addr.s_addr = htonl(INADDR_ANY); // check

		if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
			throw std::runtime_error("bind() failed.");
		if (listen(sock, 1024) < 0)
			throw std::runtime_error("listen() failed.");

		this->sockets_to_ports[sock] = port;
		std::cout << "Server listening on port " << port << "..." << std::endl;
	}
}

int	Server::acceptConnection(int listener)
{
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client = accept(listener, (struct sockaddr *)&client_addr, &client_len);
	if (client < 0) { std::cerr << "Accept() failed" << std::endl; return false; }

	fcntl(client, F_SETFL, O_NONBLOCK);
	struct epoll_event client_event;
	client_event.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP;
	client_event.data.fd = client;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client, &client_event) < 0) {
		std::cerr << "Failed to add client to epoll" << std::endl;
		close(client);
		return (false);
	}

	std::cout << "New connection on FD: " << client << std::endl;
	int port = sockets_to_ports[listener];
	this->client_fd_to_port[client] = port;
	connections[client].setFD(client);
	connections[client].setServers(config_map[port]);
	return (true);
}

void Server::closeConnection(int index)
{
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[index].data.fd, NULL);
	connections.erase(events[index].data.fd);
	close(events[index].data.fd); // check again
}

void Server::handleCGIWrite(Connection &connection, int fd)
{
	std::string body = connection.request.getBody(); 

	if (!body.empty())
	{
		ssize_t n = write(fd, body.c_str() + connection.request.cgi.offset, body.size() - connection.request.cgi.offset);
		if (n > 0)
			connection.request.cgi.offset += n;
	}

	if (connection.request.cgi.offset == body.size())
	{
		epoll_ctl(Server::epoll_fd, EPOLL_CTL_DEL, fd, NULL);
		close(fd);
	}
}

void Server::handleCGIRead(Connection &connection, int fd)
{
	char buf[RSIZE];
	ssize_t n = read(fd, buf, RSIZE - 1);

	if (n > 0)
	{
		buf[n] = 0;
		std::string currentBuffer = connection.request.cgi.getBuffer();
		currentBuffer.append(buf, n);
		connection.request.cgi.setBuffer(currentBuffer);
	}
	else if (n == 0)
	{
		epoll_ctl(Server::epoll_fd, EPOLL_CTL_DEL, fd, NULL);
		close(fd);
		waitpid(connection.request.cgi.pid, NULL, WNOHANG);
		{
			connection.response = Response(200);
			connection.response.setHeader("Content-Length", "6");
			connection.response.setHeader("Content-Type", "text/plain");
			connection.response.setBody("lhrba\n");
		}
		connection.response.setReady(1);
		Server::setEvents(connection.getFD(), EPOLLOUT, EPOLL_CTL_MOD);
		// make response
	}
	else
	{
		epoll_ctl(Server::epoll_fd, EPOLL_CTL_DEL, fd, NULL);
		close(fd);
	}
}

void Server::handleCGIIO(int index)
{
	int fd = events[index].data.fd;
	std::map<int, Connection>::iterator it = connections.begin();

	for (; it != connections.end(); ++it)
	{
		Connection &con = it->second;
		if (it->second.request.cgi.in == fd)
			handleCGIWrite(con, con.request.cgi.in);
		if (it->second.request.cgi.out == fd)
			handleCGIRead(con, con.request.cgi.out);
	}
}

void Server::setEvents(int fd, int events, int mode)
{
	struct epoll_event ev;

	ev.data.fd = fd;
	ev.events = events;
	epoll_ctl(epoll_fd, mode, fd, &ev);
}

void Server::run()
{
	std::cout << "WebServer is running..." << std::endl;
	while (serverRunning)
	{
		int nevents = epoll_wait(epoll_fd, events, MAX_EVENTS, 3000);
		if (nevents <= 0)
			continue;
		for (int i = 0; i < nevents; ++i)
		{
			int curr = events[i].data.fd;
			if (sockets_to_ports.count(curr))
				acceptConnection(curr);
			else if (connections.count(curr))
				handleConnectionIO(i);
			else
				handleCGIIO(i);
			// the close connection condition was always fullfilled because the pipe fds are non-client
			// so they were considered close/dead for the events epoll even if they had EPOLLIN/EPOLLOUT
		}
	}
	close(epoll_fd);
	std::cerr << "\nServer stopped\n";
}
