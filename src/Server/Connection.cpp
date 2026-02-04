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

	ssize_t size = recv(fd, buffer, RSIZE - 1, 0);
	if (!size)
		return Server::setEvents(fd, 0, EPOLL_CTL_DEL);
	if (size < 0)
		return Server::setEvents(fd, EPOLLERR, EPOLL_CTL_MOD);
	buffer[size] = 0;
	data = string(buffer);
	parse(data);
	request = parse.getRequest();
}

void Connection::write() {
	std::string data = getData();

	if (data.size() > offset) {
		ssize_t size = send(fd, data.c_str() + offset, std::min(static_cast<size_t>(WSIZE), data.size() - offset), 0);
		if (size > 0)
			offset += size;
		if (size < 0)
			Server::setEvents(fd, EPOLLERR, EPOLL_CTL_MOD);
	}
	if (data.size() == offset) {
		Server::setEvents(fd, EPOLLIN | EPOLLRDHUP | EPOLLHUP, EPOLL_CTL_MOD);
		if (response.getHeader("Connection") != "keep-alive")
			Server::setEvents(fd, EPOLLERR, EPOLL_CTL_MOD);
		else
			reset();
	}
}

void Connection::processRequest() {
	std::vector<ConfigBlock>::const_iterator candidate;

	if (parse.getStatus() == PARSE_FAIL) {
		response = Response(400);
		return;
	}
	candidate = request.getCandidate(getServers());
	if (candidate == getServers().end())
		response = Response(400);
	else {		
		request.setServer(*candidate);
		if (request.process(response))
			return request.setReady(0);
		response.setServer(*candidate);
		response.setReady(true);
	}
	request.setReady(0);
}

void Connection::processResponse() {
	response.process(request);
	setData(response.mkResponse());
	setDataReady(1);
	response.setReady(0);
	Server::setEvents(fd, EPOLLOUT | EPOLLRDHUP | EPOLLHUP, EPOLL_CTL_MOD);
}

void Server::handleConnectionIO(int index) {
	int fd = events[index].data.fd;
	int ev = events[index].events;
	Connection &connection = connections[fd];

	if (ev & EPOLLIN) // Read
		connection.read();
	if (connection.request.isReady()) { // Process request
		connection.processRequest();
		if (connection.request.cgi.isReady()){
			Server::setEvents(connection.request.cgi.in, EPOLLOUT, EPOLL_CTL_ADD);
			Server::setEvents(connection.request.cgi.out, EPOLLIN, EPOLL_CTL_ADD);
			// struct epoll_event ev;
			// ev.events = EPOLLOUT;
			// ev.data.ptr = &connection;
			// epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connection.request.cgi.in, &ev);
			// struct epoll_event evt;
			// evt.events = EPOLLIN;
			// evt.data.ptr = &connection;
			// epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connection.request.cgi.out, &evt);
		}
	}
	if (connection.response.isReady()) // Process response
		connection.processResponse();
	if (ev & EPOLLOUT) // Write
		connection.write();
}