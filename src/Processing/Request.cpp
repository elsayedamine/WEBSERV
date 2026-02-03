#include <Request.hpp>

Request::Request() : ready(false) {}

void Request::setServer(const ConfigBlock &s) { server = s; }

void Request::setHeader(const std::string& key, const std::string& value) {
	headers.insert(make_pair(key, value));
}

void Request::setBody(const std::string& b) { body = b; }

void Request::setMethod(const std::string &m) { method = m; }

void Request::setTarget(const std::string &t) { target = t; }

void Request::setVersion(const std::string &v) { version = v; }

void Request::setReady(bool r) { ready = r; }

const std::string &Request::getMethod() const { return method; }

const std::string& Request::getTarget() const { return target; }

const std::string& Request::getVersion() const { return version; }

const std::multimap<std::string, std::string>& Request::getHeaders() const { return headers; }

const std::string& Request::getBody() const { return body;}

bool Request::isReady() const { return ready; }

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
std::ostream& operator<<(std::ostream& os, const Request& req) {
	os << "========== REQUEST ==========\n";
	os << "Method:  " << req.getMethod() << "\n";
	os << "Target:  " << req.getTarget() << "\n";
	os << "Version: " << req.getVersion() << "\n";
	os << "\nHeaders:\n";
	
	const std::multimap<std::string, std::string>& headers = req.getHeaders();
	for (mmap_it it = headers.begin(); it != headers.end(); ++it) {
		os << "  " << it->first << ": " << it->second << "\n";
	}
	
	os << "\nBody (" << req.getBody().length() << " bytes):\n";
	if (req.getBody().length() > 0) {
		if (req.getBody().length() > 200) {
			os << req.getBody().substr(0, 200) << "...\n";
		} else {
			os << req.getBody() << "\n";
		}
	}
	os << "============================\n";
	
	return os;
}