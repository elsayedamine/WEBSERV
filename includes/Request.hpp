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

		ConfigBlock server;

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

		void parseRequest(string &data);
		std::vector<ConfigBlock>::const_iterator getCandidate(const std::vector<ConfigBlock> &candidates) const;
		Response processRequest() const;
		int Request::validateRequest() const;
		Response handleRequest(const vector<ConfigBlock> &locations) const;
};

typedef multimap<string, string>::const_iterator mmap_it;
typedef map<string, string>::const_iterator map_it;

#endif