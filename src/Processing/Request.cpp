#include <Request.hpp>
#include <main.hpp>

Request::Request(const string& m, const string& t, const string& v) : headerCount(0) {
	this->method = m;
	this->target = t;
	this->version = v;
}

void Request::setHeader(const string& key, const string& value) {
	pair<mmap_it, mmap_it> range = headers.equal_range(key);
	if (range.first == range.second)
		this->headerCount++;
	headers.insert(make_pair(key, value));
}

void Request::setBody(const string& b) { 
	body = b;
}

const string &Request::getMethod() const { 
	return method;
}

Method Request::getMethodEnum() const {
	if (method == "GET")
		return (GET);
	if (method == "POST")
		return (POST);
	if (method == "PUT")
		return (PUT);
	if (method == "DELETE")
		return (DELETE);
	return (UNKNOWN);
}


const string& Request::getTarget() const { 
	return target;
}

const string& Request::getVersion() const { 
	return version;
}

const multimap<string, string>& Request::getHeaders() const { 
	return headers;
}

string Request::getHeader(const string& key) const {  
	mmap_it it = headers.find(key);
	return it != headers.end() ? it->second : "";
}

const string& Request::getBody() const { 
	return body;
}

ostream& operator<<(ostream& os, const Request& req) {
	os << "=== Request Debug Info ===\n";
	os << "Method: " << req.getMethod() << "\n";
	os << "Target: " << req.getTarget() << "\n";
	os << "Version: " << req.getVersion() << "\n";
	os << "Header Count: " << req.headerCount << "\n";
	os << "Headers:\n";

	for (multimap<string, string>::const_iterator it = req.getHeaders().begin();
		 it != req.getHeaders().end(); ++it)
		os << "  " << it->first << ": " << it->second << "\n";

	os << "Body:\n" << req.getBody() << "\n";
	os << "==========================\n";

	return os;
}
