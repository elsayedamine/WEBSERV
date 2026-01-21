#include <Response.hpp>
#include <Methods.hpp>
#include <Server.hpp>
#include <Utils.hpp>
#include <unistd.h>

std::vector<ConfigBlock>::const_iterator Request::getCandidate(const std::vector<ConfigBlock> &candidates) const {
	std::vector<ConfigBlock>::const_iterator it = candidates.begin();
	for (it; it != candidates.end(); ++it) {
		if (!it->server_name.empty() && it->server_name[0] == getHeader("Host"))
			return it;
	}
	return candidates.end();
}

void 

void Server::handleConnectionIO(int fd, int events) {
	std::vector<ConfigBlock>::const_iterator candidate;
	string data;
	vector<ConfigBlock> candidates = config_map[client_fd_to_port[connection.getFD()]];

	{ // processData
		connection.request.parseRequest(data);
		candidate = connection.request.getCandidate(candidates);
		if (candidate == candidates.end())
			connection.response = Response(400);
		else
			connection.response = connection.request.processRequest();
	}
	connection.response.processResponse(connection.request, *candidate);
	connection.response.sendResponse(connection.getFD());
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, connection.getFD(), NULL);
	close(connection.getFD());
}