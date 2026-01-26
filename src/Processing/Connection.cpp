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

void handleCGIWrite(int pipe_fd)
{
	(void)pipe_fd;
// 	int client_fd = Server::pipe_to_client[pipe_fd];
// 	// write request body to CGI stdin
// 	ssize_t n = write(pipe_fd,
// 					Server::client_body[client_fd].data(),
// 					Server::client_body[client_fd].size());
// 	if (n > 0)
// 	{
// 		// remove written part
// 		Server::client_body[client_fd].erase(0, n);
// 		if (Server::client_body[client_fd].empty())
// 		{
// 			close(pipe_fd);
// 			epoll_ctl(Server::epoll_fd, EPOLL_CTL_DEL, pipe_fd, NULL);
// 		}
// 	}
}

string handleCGIRead(int pipe_fd)
{
	(void)pipe_fd;
// 	string output;
// 	char buf[4096];
// 	int client_fd = Server::pipe_to_client[pipe_fd];
// 	ssize_t n = read(pipe_fd, buf, sizeof(buf));
// 	if (n > 0)
// 	{
// 		Server::cgi_output[client_fd].append(buf, n);
// 	}
// 	else if (n == 0)
// 	{
// 		close(pipe_fd);
// 		epoll_ctl(Server::epoll_fd, EPOLL_CTL_DEL, pipe_fd, NULL);
// 		pid_t pid = Server::pipe_to_pid[pipe_fd];
// 		waitpid(pid, NULL, WNOHANG);
// 		// now CGI response is complete
// 		// build_http_response(client_fd);
// 	}
	return std::string(); 
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
		// after this step we need to fill the cgi within the connection class
		connection.response.processResponse(connection.request, *candidate);
		connection.response.mkResponse();
		// how can you make a response while the cgi is not finished processing yet which is the body of the response
	}
	if (ev & EPOLLOUT) { // Write
		// check use send() instead
		ssize_t size = write(fd, connection.response.getData().c_str(), WSIZE);
		connection.response.setData(connection.response.getData().substr(size));
	}
	if (connection.response.getHeader("Connection") != "keep-alive") { // Close
		closeConnection(index);
	}
}