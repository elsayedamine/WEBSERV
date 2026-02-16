#include <Server.hpp>

int serverRunning = 1;
int Server::epoll_fd = -1;

Server::~Server()
{
	for (std::map<int,int>::iterator it = sockets_to_ports.begin(); it != sockets_to_ports.end(); ++it)
		if (it->first >= 0)
			close(it->first);
	for (std::map<int, Connection>::iterator it = connections.begin(); it != connections.end(); ++it)
	{
		Connection &con = it->second;
		if (con.getFD() >= 0) 
		{ 
			epoll_ctl(epoll_fd, EPOLL_CTL_DEL, con.getFD(), NULL);
			close(con.getFD()); 
		}
		if (con.request.cgi.in >= 0)
		{
			epoll_ctl(epoll_fd, EPOLL_CTL_DEL, con.request.cgi.in, NULL);
			close(con.request.cgi.in);
			con.request.cgi.in = -1;
		}
		if (con.request.cgi.out >= 0)
		{
			epoll_ctl(epoll_fd, EPOLL_CTL_DEL, con.request.cgi.out, NULL);
			close(con.request.cgi.out);
			con.request.cgi.out = -1;
		}
	}
	connections.clear();
	if (epoll_fd >= 0)
		close(epoll_fd);
}

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

struct sockaddr_in resolveAddress(const std::string &ip, int port)
{
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    struct addrinfo criteria, *res = NULL;
    std::memset(&criteria, 0, sizeof(criteria));

    criteria.ai_family = AF_INET; // ai stands for addr info 
    criteria.ai_socktype = SOCK_STREAM;
    criteria.ai_flags = AI_PASSIVE; // indicate the socket will be used for binding

    int ret = getaddrinfo(ip.c_str(), NULL, &criteria, &res); // ip_string -> sockaddr linked list
    if (ret != 0 || res == NULL)
        throw std::runtime_error(std::string("Invalid IP: ") + gai_strerror(ret));

    struct sockaddr_in *addr_in = (struct sockaddr_in *)res->ai_addr;
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    addr.sin_addr   = addr_in->sin_addr;

    freeaddrinfo(res);
    return addr;
}

void Server::SetupSockets()
{
	std::map<int, std::vector<ConfigBlock> >::iterator it;
	for (it = config_map.begin(); it != config_map.end(); ++it) {
		int port  = it->first;
		std::vector<ConfigBlock> &servers = it->second;
		for (std::size_t i = 0; i < servers.size(); ++i)
		{
			int sock;
			int opt = 1;
			if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
				throw std::runtime_error("socket() failed.");

			if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
				throw std::runtime_error("setsockopt SO_REUSEADDR failed");
			if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0)
				throw std::runtime_error("setsockopt SO_REUSEPORT failed");

			struct sockaddr_in addr = resolveAddress(servers[i].host, port);

			if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
				close(sock); throw std::runtime_error("bind() failed."); }
			if (listen(sock, 1024) < 0)
				throw std::runtime_error("listen() failed.");

			this->sockets_to_ports[sock] = port;
			std::cout << "Server listening on port " << port << "..." << std::endl;
		}
	}
}

int	Server::acceptConnection(int listener)
{
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client = accept(listener, (struct sockaddr *)&client_addr, &client_len);
	if (client < 0) { std::cerr << "Accept() failed" << std::endl; return false; }

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
	int fd = events[index].data.fd;
	Connection &con = connections[fd];
	if (con.request.cgi.in > 0) {
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, con.request.cgi.in, NULL);
		close(con.request.cgi.in);
	}
	if (con.request.cgi.out > 0) {
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, con.request.cgi.out, NULL);
		close(con.request.cgi.out);
	}
	if (con.request.cgi.pid > 0) {
		int status;
		waitpid(con.request.cgi.pid, &status, WNOHANG);
	}
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
	close(fd);
	connections.erase(fd);
}

void Server::setEvents(int fd, int events, int mode)
{
	struct epoll_event ev;

	if (fd == -1)
		return;
	ev.data.fd = fd;
	ev.events = events;
	epoll_ctl(epoll_fd, mode, fd, &ev);
}

void Server::run()
{
	std::cout << "WebServer is running..." << std::endl;
	while (serverRunning)
	{
		int nevents = epoll_wait(epoll_fd, events, MAX_EVENTS, MAX_WAIT);
		if (nevents <= 0) {
			
			continue;
		}
		for (int i = 0; i < nevents; ++i)
		{
			int curr = events[i].data.fd;
			if (sockets_to_ports.count(curr))
				acceptConnection(curr);
			else if (connections.count(curr))
				handleConnectionIO(i);
			else
				handleCGIIO(i);
		}
	}
	std::cerr << "\nServer stopped\n";
}
