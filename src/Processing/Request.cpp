#include <Request.hpp>

Request::Request() : parse(*this) {};

void Request::setServer(const ConfigBlock &s) { server = s; }

void Request::setHeader(const string& key, const string& value) {
	pair<mmap_it, mmap_it> range = headers.equal_range(key);
	if (range.first == range.second)
		this->headerCount++;
	headers.insert(make_pair(key, value));
}

void Request::setBody(const string& b) { body = b; }

const string &Request::getMethod() const { return method; }

const string& Request::getTarget() const { return target; }

const string& Request::getVersion() const { return version; }

const multimap<string, string>& Request::getHeaders() const { return headers; }

const string& Request::getBody() const { return body;}

string Request::getHeader(const string& key) const {  
	mmap_it it = headers.find(key);
	return it != headers.end() ? it->second : "";
}

Method Request::getMethodEnum() const {
	if (method == "GET") return (GET);
	if (method == "POST") return (POST);
	if (method == "PUT") return (PUT);
	if (method == "DELETE") return (DELETE);
	return (UNKNOWN);
}
