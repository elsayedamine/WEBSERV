#include <Response.hpp>
#include <Methods.hpp>
#include <Utils.hpp>
#include <unistd.h>
#include <main.hpp>

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
	cout << text << endl;
	write(fd, text.c_str(), text.size());
}

void handleConnection(int fd, const ConfigBlock &server) {
	string data;
	Request *request;
	Response *response;

	{ // Read from fd into a string object
		char buffer[1024];
		ssize_t readSize;
		
		while ((readSize = read(fd, buffer, 1024)) > 0)
			data.append(buffer, readSize);
	}
	{ // Parse request
		request = parseRequest(data);
	}
	{ // Process request
		response = processRequest(*request, server);
	}
	{ // Send response
		sendResponse(fd, *response);
	}
}