#include <Response.hpp>

// Response class method implementations
Response::Response(int c) : code(c) {

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

const map<string, string>& Response::getHeaders() const { 
	return headers; 
}

const string& Response::getBody() const {
	return body;
}
