#include "Response.hpp"

// Response class method implementations
Response::Response(int c, const string& m) {
	this->code = c;
	this->message = m;
}

void Response::setHeader(string& key, string& value) {
	headers[key] = value;
}

void Response::setBody(const string& b) {
	body = b;
}

int Response::getCode() const {
	return code;
}

const string& Response::getMessage() const {
	return message;
}

string Response::getHeader(const string& key) const {
	map<string, string>::const_iterator it = headers.find(key);
	return it != headers.end() ? it->second : "";
}

const string& Response::getBody() const {
	return body;
}
