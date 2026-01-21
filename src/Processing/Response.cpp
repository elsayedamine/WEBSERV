#include <Response.hpp>
#include <Utils.hpp>

void Response::sendResponse(int fd) const {
	string text = "HTTP/1.1 ";

	{ // Status code and message
		text.append(num_to_string(code) + ' ');
		text.append(getCodeMessage(code) + "\r\n");
	}
	{ // Headers
		map<string, string>::const_iterator it = headers.begin();

		while (it != headers.end()) {
			text.append(it->first + ": " + it->second + "\r\n");
			++it;
		}
	}
	if (!body.empty()) { // Body
		text.append("\r\n");
		text.append(body);
	}
	// write(fd, text.c_str(), text.size());
}

std::string Response::getCodeMessage(int code) const {
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
