#include <Response.hpp>
#include <Methods.hpp>
#include <Server.hpp>
#include <Utils.hpp>
#include <unistd.h>

std::vector<ConfigBlock>::const_iterator Request::getCandidate(const std::vector<ConfigBlock> &candidates) const {
	std::vector<ConfigBlock>::const_iterator it = candidates.begin();
	if (it->server_name.empty())
		return candidates.end();
	for (; it != candidates.end(); ++it) {
		if (std::find(it->server_name.begin(), it->server_name.end(), getHeader("Host")) != it->server_name.end())
			return it;
	}
	return candidates.end();
}

void Server::handleWrite(int index) {
	Connection & connection = connections[events[index].data.fd];
	int wr = connection.write();
	if (wr < 0)
		return (closeConnection(index));
	if (wr > 0 && connection.response.getHeader("Connection") != "keep-alive")
		return (closeConnection(index));
}

void Connection::reset() {
	offset = 0;
	data.clear();
	parse = Parser(servers);
	request = Request();
	request.setReady(0);
	response = Response();
	response.setReady(0);
	timeout = TIMEOUT;
	timed_out = false;
	Server::setEvents(fd, EPOLLIN | EPOLLRDHUP | EPOLLHUP, EPOLL_CTL_MOD);
}

void Connection::read() {
	char buffer[RSIZE];
	std::string data;

	timeout = TIMEOUT;
	ssize_t size = recv(fd, buffer, RSIZE, MSG_NOSIGNAL | MSG_DONTWAIT);
	if (!size) {
		request.setReady(1);
		return Server::setEvents(fd, 0, EPOLL_CTL_DEL);
	}
	if (size < 0)
		return Server::setEvents(fd, EPOLLERR, EPOLL_CTL_MOD);
	data = std::string(buffer, size);
	parse(data);
	request = parse.getRequest();
}

int Connection::write() {
	std::string data = getData();

	timeout = TIMEOUT;
	if (data.size() > offset) {
		ssize_t size = ::write(fd, data.c_str() + offset, std::min(static_cast<size_t>(WSIZE), data.size() - offset));
		if (size > 0)
			offset += size;
		if (size <= 0)
			return (-1);
	}
	if (data.size() == offset) {
		if (response.getHeader("Connection") == "keep-alive")
			reset();
		return (1);
	}
	return (0);
}

void Connection::processRequest() {
	std::vector<ConfigBlock>::const_iterator candidate;

	candidate = request.getCandidate(getServers());
	if (parse.getStatus() == PARSE_FAIL || candidate == getServers().end())
		response = Response(parse.getErr());
	else {
		request.setServer(*candidate);
		response.setServer(*candidate);
		if (!request.getMethodEnum())
			response = Response(501);
		else if (request.process(response))
			return request.setReady(0);
		response.setReady(true);
	}
	request.setReady(0);
}

void Connection::processResponse() {
	response.process(request);
	setData(response.mkResponse());
	response.setReady(0);
	Server::setEvents(fd, EPOLLOUT | EPOLLRDHUP | EPOLLHUP, EPOLL_CTL_MOD);
}

void Server::handleConnectionIO(int index) {
	int fd = events[index].data.fd;
	int ev = events[index].events;
	Connection &connection = connections[fd];

	if (ev & EPOLLERR)
		return (closeConnection(index));
	if (ev & EPOLLIN)
		connection.read();
	if (connection.timed_out) {
		if (ev & EPOLLOUT)
			handleWrite(index);
		return;
	}
	if (connection.request.isReady()) // Process request
		connection.processRequest();
	if (connection.response.isReady()) // Process response
		connection.processResponse();
	if (ev & EPOLLOUT) // Write
		return (handleWrite(index));
}
