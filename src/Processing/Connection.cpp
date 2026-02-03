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

void Server::handleCGIWrite(int pipe_fd)
{
	int client_fd = Server::connect[pipe_fd];

	Request &req = connections[client_fd].request;
	std::string body = req.getBody(); 

	if (!body.empty())
	{
		ssize_t n = write(pipe_fd, body.c_str(), body.size());
		if (n > 0)
			req.setBody(body.substr(n));
	}

	if (req.getBody().empty())
	{
		epoll_ctl(Server::epoll_fd, EPOLL_CTL_DEL, pipe_fd, NULL);
		close(pipe_fd);
	}
}
void Server::handleCGIRead(int pipe_fd)
{
	int client_fd = Server::connect[pipe_fd];
	char buf[RSIZE];
	ssize_t n = read(pipe_fd, buf, RSIZE);

	if (n > 0)
	{
		std::string currentBody = connections[client_fd].response.getBody();
		currentBody.append(buf, n);
		connections[client_fd].response.setBody(currentBody);
	}
	else if (n == 0)
	{
		epoll_ctl(Server::epoll_fd, EPOLL_CTL_DEL, pipe_fd, NULL);
		close(pipe_fd);
		waitpid(Server::cgi[client_fd].pid, NULL, WNOHANG);

		connections[client_fd].response.mkResponse(); // change the place

		int pipe_in = Server::cgi[client_fd].in;
		Server::connect.erase(pipe_fd);
		Server::connect.erase(pipe_in);
		Server::cgi.erase(client_fd);
	}
}

void	Server::handleCGIIO(int fd)
{
	int client_fd = connect[fd];
	CGIHandle handler = cgi[client_fd];

	if (fd == handler.in)
		handleCGIWrite(fd);
	if (fd == handler.out)
		handleCGIRead(fd);
}

void Server::setEvents(int &fd, int events) {
	struct epoll_event ev;

	ev.data.fd = fd;
	ev.events = events;
	epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev);
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
	if (size < 0)
		return Server::setEvents(fd, EPOLLERR);
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
			Server::setEvents(fd, EPOLLERR);	
	}
	if (getData().empty()) {
		Server::setEvents(fd, EPOLLIN | EPOLLRDHUP | EPOLLHUP);
		if (response.getHeader("Connection") != "keep-alive")
			Server::setEvents(fd, EPOLLERR);
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
		response = request.process();
		response.setServer(*candidate);
		response = request.process();
		response.setReady(true);
	}
	request.setReady(0);
}

void Connection::processResponse() {
	response.process(request);
	setData(response.mkResponse());
	setDataReady(1);
	Server::setEvents(fd, EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLHUP);
}

void Server::handleConnectionIO(int index) {
	int fd = events[index].data.fd;
	int ev = events[index].events;
	Connection &connection = connections[fd];

	if (ev & EPOLLIN) // Read
		connection.read();
	if (connection.request.isReady()) // Process request
		connection.processRequest();
	if (connection.response.isReady()) // Process response
		connection.processResponse();
	if (ev & EPOLLOUT && connection.dataReady()) // Write
		connection.write();
}