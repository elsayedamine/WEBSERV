#include <Response.hpp>
#include <Methods.hpp>
#include <Server.hpp>
#include <Utils.hpp>
#include <unistd.h>

std::vector<ConfigBlock>::const_iterator Request::getCandidate(const std::vector<ConfigBlock> &candidates) const {
	std::vector<ConfigBlock>::const_iterator it = candidates.begin();
	for (; it != candidates.end(); ++it) {
		if (!it->server_name.empty() && it->server_name[0] == getHeader("Host"))
			return it;
	}
	return candidates.end();
}

void Server::handleCGIIO(int index) {
	(void)index;
}

void Server::handleConnectionIO(int index) {
	int fd = events[index].data.fd;
	int ev = events[index].events;
	vector<ConfigBlock> candidates = config_map[client_fd_to_port[fd]];
	Connection connection = connections[fd];

	if (ev & EPOLLIN) { // Read
		char buffer[RSIZE];
		ssize_t size = recv(fd, buffer, RSIZE, 0);
		string data;
		
		buffer[size] = 0;
		data = string(buffer);
		connection.parse(data);
	}
	if (!connection.response.isReady()) { // Process
		std::vector<ConfigBlock>::const_iterator candidate;

		candidate = connection.request.getCandidate(candidates);
		connection.request.setServer(*candidate);
		connection.response = (candidate == candidates.end()) ? Response(400) : connection.request.processRequest();
		connection.response.processResponse(connection.request, *candidate);
		connection.response.mkResponse();
	}
	if (ev & EPOLLOUT) { // Write
		// check use send() instead
		ssize_t size = write(fd, connection.response.getData().c_str(), WSIZE);
		connection.response.setData(connection.response.getData().substr(size));
	}
	if (connection.response.getHeader("Connection") != "keep-alive")
		closeConnection(index);
}