#include <Response.hpp>

Response::Response(int c) : code(c) {

}

void Response::setHeader(const string& key, const string& value) {
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
