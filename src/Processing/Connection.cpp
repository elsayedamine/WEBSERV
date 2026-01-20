#include <Response.hpp>
#include <Methods.hpp>
#include <Utils.hpp>
#include <unistd.h>
#include <main.hpp>

std::string getCodeMessage(int code) {
	static map<int, std::string> messages;
	if (messages.empty()) {
		messages[200] = "OK";
		messages[201] = "Created";
		messages[204] = "No Content";
		messages[301] = "Moved Permanently";
		messages[302] = "Found";
		messages[304] = "Not Modified";
		messages[400] = "Bad Request";
		messages[403] = "Forbidden";
		messages[404] = "Not Found";
		messages[405] = "Method Not Allowed";
		messages[411] = "Length Required";
		messages[413] = "Payload Too Large";
		messages[414] = "URI Too Long";
		messages[415] = "Unsupported Media Type";
		messages[500] = "Internal Server Error";
		messages[501] = "Not Implemented";
		messages[502] = "Bad Gateway";
		messages[505] = "HTTP Version Not Supported";
	}

	return (messages[code]);
}

void sendResponse(int fd, Response &response) {
	string text = "HTTP/1.1 ";

	{ // Status code and message
		text.append(num_to_string(response.getCode()) + ' ');
		text.append(getCodeMessage(response.getCode()) + "\r\n");
	}
	{ // Headers
		map<string, string> headers = response.getHeaders();
		map<string, string>::const_iterator it = headers.begin();

		while (it != headers.end()) {
			text.append(it->first + ": " + it->second + "\r\n");
			++it;
		}
	}
	if (!response.getBody().empty()) { // Body
		text.append("\r\n");
		text.append(response.getBody());
	}
	write(fd, text.c_str(), text.size());
}

int getServer(const std::vector<ConfigBlock> &candidates, Request &request) {

	for (size_t i = 0; i < candidates.size(); i++) {
		if (candidates[i].server_name[0] == request.getHeader("Host"))
			return (i);
	}
	return (-1);
}

void Server::handleConnection(int fd, const std::vector<ConfigBlock> &candidates) {
	string data;
	Request request;
	Response response;

	{ // Read from fd into a string object
		char buffer[1024];
		ssize_t readSize;
		
		while ((readSize = read(fd, buffer, 1024)) > 0)
			data.append(buffer, readSize);
	}
	{
		int index;

		request = parseRequest(data);
		index = getServer(candidates, request);
		if (index == -1)
			response = Response(400);
		else
			response = processRequest(request, candidates[index]);
	}
	sendResponse(fd, response);
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
	close(fd);
}