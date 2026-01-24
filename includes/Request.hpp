#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <vector>
#include <iostream>
#include <map>
#include <ConfigBlock.hpp>
#include <Response.hpp>
#include <Parse.hpp>
#include <ConfigBlock.hpp>

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
		int headerCount;

		ConfigBlock server;

	public:
		Parse parse;

		Request();

		void setHeader(const string &key, const string &value);
		void setBody(const string &b);
		void setServer(const ConfigBlock &server);

		const string &getMethod() const;
		Method getMethodEnum() const;
		const string &getTarget() const;
		const string &getVersion() const;
		const multimap<string, string> &getHeaders() const;
		string getHeader(const string &key) const;
		const string &getBody() const;

		std::vector<ConfigBlock>::const_iterator getCandidate(const std::vector<ConfigBlock> &candidates) const;
		Response processRequest();
		int validateRequest() const;
		Response handleRequest(const vector<ConfigBlock> &locations) const;
};

typedef multimap<string, string>::const_iterator mmap_it;
typedef map<string, string>::const_iterator map_it;

#endif