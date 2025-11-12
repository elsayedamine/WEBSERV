#include <Server.hpp>

Server::Server(const Configuration & conf) : config(conf)
{
	std::set<std::pair<int, std::string>> listeners;
	const std::vector<ConfigBlock> &servers = config.getServers();
	for (std::size_t i = 0; i < servers.size(); ++i) {
		int port = servers[i].port;
		std::string host = servers[i].host;
		listeners.insert(std::make_pair(port, host));
	}
}
