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

void Connection::reset() {
	offset = 0;
	data.clear();
	setDataReady(0);
	parse = Parser();
	request = Request();
	request.setReady(false);
	response = Response();
	response.setReady(0);
}
void Connection::read() {
	char buffer[RSIZE];
	string data;

	ssize_t size = recv(fd, buffer, RSIZE, 0);
	if (!size)
		return Server::setEvents(fd, 0, EPOLL_CTL_MOD);
	if (size < 0)
		return Server::setEvents(fd, EPOLLERR, EPOLL_CTL_MOD);
	buffer[size] = 0;
	data = string(buffer);
	parse(data);
	request = parse.getRequest();
}

void Connection::write() {
	std::string data = getData();

	if (!data.empty()) {
		ssize_t size = send(fd, data.c_str() + offset, std::min(static_cast<size_t>(WSIZE), data.size()), 0);
		if (size > 0)
			offset += size;
		if (size < 0)
			Server::setEvents(fd, EPOLLERR, EPOLL_CTL_MOD);
	}
	if (getData().empty()) {
		Server::setEvents(fd, EPOLLIN | EPOLLRDHUP | EPOLLHUP, EPOLL_CTL_MOD);
		if (response.getHeader("Connection") != "keep-alive")
			Server::setEvents(fd, EPOLLERR, EPOLL_CTL_MOD);
		else
			reset();
	}
}

void Connection::processRequest() {
	std::vector<ConfigBlock>::const_iterator candidate;

	candidate = request.getCandidate(getServers());
	if (candidate == getServers().end())
		response = Response(400);
	else {		
		request.setServer(*candidate);
		if (request.process(response))
			return request.setReady(0);
		response.setServer(*candidate);
		response.process(request);
		response.setReady(true);
	}
	request.setReady(0);
}

void Connection::processResponse() {
	response.process(request);
	setData(response.mkResponse());
	setDataReady(1);
	Server::setEvents(fd, EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLHUP, EPOLL_CTL_MOD);
}

void Server::handleConnectionIO(int index) {
	int fd = events[index].data.fd;
	int ev = events[index].events;
	Connection &connection = connections[fd];

	if (ev & EPOLLIN) // Read
		connection.read();
	if (connection.request.isReady()) { // Process request
		connection.processRequest();
		if (connection.request.cgi.isReady())
			events[index].data.ptr = &connection;
	}
	if (connection.response.isReady()) // Process response
		connection.processResponse();
	if (ev & EPOLLOUT && connection.dataReady()) // Write
		connection.write();
}