#include <Request.hpp>

Request::Request() {}

void Request::setServer(const ConfigBlock &s) { server = s; }

void Request::setHeader(const std::string& key, const std::string& value) {
	std::pair<mmap_it, mmap_it> range = headers.equal_range(key);
	if (range.first == range.second)
		this->headerCount++;
	headers.insert(make_pair(key, value));
}

void Request::setBody(const std::string& b) { body = b; }

void Request::setMethod(const std::string &m) { method = m; }

void Request::setTarget(const std::string &t) { target = t; }

void Request::setVersion(const std::string &v) { version = v; }

const std::string &Request::getMethod() const { return method; }

const std::string& Request::getTarget() const { return target; }

const std::string& Request::getVersion() const { return version; }

const std::multimap<std::string, std::string>& Request::getHeaders() const { return headers; }

const std::string& Request::getBody() const { return body;}

std::string Request::getHeader(const std::string& key) const {  
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
