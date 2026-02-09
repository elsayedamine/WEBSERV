#include <Utils.hpp>
#include <Response.hpp>

int Response::getCode() const { return code; }

void Response::setServer(const ConfigBlock &s) { server = s; }

const ConfigBlock &Response::getServer() const { return server; }

void Response::setHeader(const std::string& key, const std::string& value) {
	if (value.empty()) return;
	headers[key] = value;
}

void Response::setBody(const std::string& b) { body = b; }

void Response::setStatus(int c) { code = c; }

void Response::setReady(int r) { ready = (r != 0); }

int Response::isReady() const { return ready; }

const std::map<std::string, std::string>& Response::getHeaders() const { return headers; }

const std::string& Response::getBody() const { return body; }

std::string Response::getHeader(const std::string& key) const {
	std::map<std::string, std::string>::const_iterator it = headers.find(key);
	return it != headers.end() ? it->second : "";
}

std::string Response::mkResponse() {
	std::string buffer = "HTTP/1.1 ";

	{ // Status code and message
		buffer.append(num_to_string(code) + ' ');
		buffer.append(getCodeMessage(code) + "\r\n");
	}
	{ // Headers
		std::map<std::string, std::string>::const_iterator it = headers.begin();

		while (it != headers.end()) {
			buffer.append(it->first + ": " + it->second + "\r\n");
			++it;
		}
	}
	if (!body.empty()) { // Body
		buffer.append("\r\n");
		buffer.append(body);
	}
	return (buffer);
}

std::string Response::getCodeMessage(int code) const {
	static std::map<int, std::string> messages;
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
		messages[408] = "Request Timeout";
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
