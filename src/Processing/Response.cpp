#include <Utils.hpp>
#include <Response.hpp>

int Response::getCode() const { return code; }

void Response::setHeader(const string& key, const string& value) {
	if (value.empty()) return;
	headers[key] = value;
}

void Response::setBody(const string& b) { body = b; }

void Response::setData(const string& d) { data = d; }

int Response::isReady() const { return ready; }

const map<string, string>& Response::getHeaders() const { return headers; }

const string& Response::getBody() const { return body; }

const string& Response::getData() const { return data; }

string Response::getHeader(const string& key) const {
	map<string, string>::const_iterator it = headers.find(key);
	return it != headers.end() ? it->second : "";
}

void Response::mkResponse() {
	string buffer = "HTTP/1.1 ";

	{ // Status code and message
		buffer.append(num_to_string(code) + ' ');
		buffer.append(getCodeMessage(code) + "\r\n");
	}
	{ // Headers
		map<string, string>::const_iterator it = headers.begin();

		while (it != headers.end()) {
			buffer.append(it->first + ": " + it->second + "\r\n");
			++it;
		}
	}
	if (!body.empty()) { // Body
		buffer.append("\r\n");
		buffer.append(body);
	}
	setData(buffer);
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
