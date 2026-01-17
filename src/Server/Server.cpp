#include <Server.hpp>

Server::Server(const Configuration & conf) : config(conf)
{
	const std::vector<ConfigBlock> &servers = config.getServers();

	for (std::size_t i = 0; i < servers.size(); ++i) {
		int port = servers[i].port;
		std::string host = servers[i].host;
		config_map[port].push_back(servers[i]);
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
		memset(&addr, 0, sizeof(addr));
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

void	Server::run()
{
	struct epoll_event events[128];
	std::cout << "WebServer is running..." << std::endl;
	while (1)
	{
		int nevents = epoll_wait(epoll_fd, events, 128, -1);
		if (nevents < 0) continue;
		for (int i = 0; i < nevents; ++i) {
			int curr = events[i].data.fd;
			if (sockets_to_ports.find(curr) != sockets_to_ports.end())
			{
				if (accept_new_connection(curr) == false)
					continue ;
			}
			else
			{
				int port = client_fd_to_port[curr];
				std::vector<ConfigBlock>& candidates = config_map[port];
				handleConnection(curr, candidates);
				// epoll_ctl(DEL) should be called inside the fct for cleaning
				// and better use the recv for the sockets
			}
		}
	}
}

int	Server::accept_new_connection(int listener)
{
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int client = accept(listener, (struct sockaddr *)&client_addr, &client_len);
	if (client < 0) { std::cerr << "Accept failed" << std::endl; return false; }

	fcntl(client, F_SETFL, O_NONBLOCK);
	struct epoll_event client_event;
	client_event.events = EPOLLIN;
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
