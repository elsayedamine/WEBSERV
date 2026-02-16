#include <Server.hpp>

void Server::cleanupCGI(Connection &con)
{
    if (con.request.cgi.in > 0)
		{ close(con.request.cgi.in); con.request.cgi.in = -1; }
    if (con.request.cgi.out > 0)
		{ close(con.request.cgi.out); con.request.cgi.out = -1; }
    if (con.request.cgi.pid > 0)
		{ waitpid(con.request.cgi.pid, NULL, WNOHANG); con.request.cgi.pid = -1; }
}

pid_t Server::handleCGIExit(pid_t &cgi_pid, Response &response)
{
	int status;
	pid_t ret = waitpid(cgi_pid, &status, WNOHANG);

	if (ret == 0)
		return 0;
	else if (ret == -1)
	{
		response = Response(500);
		response.setReady(1);
		cgi_pid = -1;
		return -1;
	} 
	else
	{
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
			response = Response(500);
			response.setReady(1);
		}
		return ret;
	}
}

void Server::handleCGIWrite(Connection &connection, int &fd)
{
	std::string body = connection.request.getBody(); 

	if (!body.empty())
	{
		ssize_t n = write(fd, body.c_str() + connection.request.cgi.offset, body.size() - connection.request.cgi.offset);
		if (n > 0)
			connection.request.cgi.offset += n;
	}

	if (connection.request.cgi.offset == body.size())
	{
		epoll_ctl(Server::epoll_fd, EPOLL_CTL_DEL, fd, NULL);
		close(fd);
		fd = -1;
	}
}

void Server::handleCGIRead(Connection &connection, int &fd)
{
	char buf[RSIZE];
	ssize_t n = read(fd, buf, RSIZE);

	if (n > 0)
	{
		std::string currentBuffer = connection.request.cgi.getBuffer();
		currentBuffer.append(buf, n);
		connection.request.cgi.setBuffer(currentBuffer);
	}
	else if (n == 0)
	{
		epoll_ctl(Server::epoll_fd, EPOLL_CTL_DEL, fd, NULL);
		close(fd);
		fd = -1;
		pid_t ret = handleCGIExit(connection.request.cgi.pid, connection.response);
		if (ret <= 0)
			return ;
		if (!connection.response.isReady())
		{
			int parse = connection.request.cgi.parse(connection.response);
			if (parse <= 0) {
				connection.response = parse == -1 ? Response(500) : connection.response;
				connection.response.setReady(1);
			}
		}
		Server::setEvents(connection.getFD(), EPOLLOUT, EPOLL_CTL_MOD);
	}
	else
	{
		epoll_ctl(Server::epoll_fd, EPOLL_CTL_DEL, fd, NULL);
		close(fd);
		fd = -1;
	}
}

void Server::handleCGIIO(int index)
{
	int fd = events[index].data.fd;
	std::map<int, Connection>::iterator it = connections.begin();

	for (; it != connections.end(); ++it)
	{
		Connection &con = it->second;
		if (it->second.request.cgi.in == fd) {
			con.timeout = TIMEOUT;
			handleCGIWrite(con, con.request.cgi.in);
		}
		if (it->second.request.cgi.out == fd) {
			con.timeout = TIMEOUT;
			handleCGIRead(con, con.request.cgi.out);
		}
	}
}
