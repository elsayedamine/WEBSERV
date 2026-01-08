#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <vector>
#include <iostream>
#include <map>

using namespace std;

enum Method {
	UNKNOWN,
	GET,
	POST,
	PUT,
	DELETE
};

class Request {
	private:
		string method;
		string target;
		string version;
		multimap<string, string> headers; 
		string body; 

	public:
		int headerCount;
		Request() {};
		Request(const string &m, const string &t, const string &v);
		void setHeader(const string &key, const string &value);
		void setBody(const string &b);
		const string &getMethod() const;
		Method getMethodEnum() const;
		const string &getTarget() const;
		const string &getVersion() const;
		const multimap<string, string> &getHeaders() const;
		string getHeader(const string &key) const;
		const string &getBody() const;
};

typedef multimap<string, string>::const_iterator mmap_it;
typedef map<string, string>::const_iterator map_it;

// --- Inline implementations ---
inline Request::Request(const string& m, const string& t, const string& v) : headerCount(0) {
	this->method = m;
	this->target = t;
	this->version = v;
}

inline void Request::setHeader(const string& key, const string& value) {
	pair<mmap_it, mmap_it> range = headers.equal_range(key);
	if (range.first == range.second)
		this->headerCount++;
	headers.insert(make_pair(key, value));
}

inline void Request::setBody(const string& b) { 
	body = b;
}

inline const string &Request::getMethod() const { 
	return method;
}

inline Method Request::getMethodEnum() const {
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


inline const string& Request::getTarget() const { 
	return target;
}

inline const string& Request::getVersion() const { 
	return version;
}

inline const multimap<string, string>& Request::getHeaders() const { 
	return headers;
}

inline string Request::getHeader(const string& key) const {  
	mmap_it it = headers.find(key);
	return it != headers.end() ? it->second : "";
}

inline const string& Request::getBody() const { 
	return body;
}

inline ostream& operator<<(ostream& os, const Request& req) {
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

#endif // REQUEST_HPP
