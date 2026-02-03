#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <vector>
#include <iostream>
#include <map>
#include <Response.hpp>
#include <CGI.hpp>

enum Method {
	UNKNOWN,
	GET,
	POST,
	PUT,
	DELETE
};

class Request {
	private:
		std::string method;
		std::string target;
		std::string version;
		std::multimap<std::string, std::string> headers; 
		std::string body;
		bool ready;

		ConfigBlock server;
		
	public:
		Request();
		CGI cgi;

		void setHeader(const std::string &key, const std::string &value);
		void setBody(const std::string &b);
		void setServer(const ConfigBlock &server);
		void setMethod(const std::string &m);
		void setTarget(const std::string &t);
		void setVersion(const std::string &v);
		void setReady(bool r);

		const std::string &getMethod() const;
		Method getMethodEnum() const;
		const std::string &getTarget() const;
		const std::string &getVersion() const;
		const std::multimap<std::string, std::string> &getHeaders() const;
		std::string getHeader(const std::string &key) const;
		const std::string &getBody() const;
		bool isReady() const;

		std::vector<ConfigBlock>::const_iterator getCandidate(const std::vector<ConfigBlock> &candidates) const;
		int validateRequest() const;
		int process(Response &response);
};

typedef std::multimap<std::string, std::string>::const_iterator mmap_it;
typedef std::map<std::string, std::string>::const_iterator map_it;

std::ostream& operator<<(std::ostream& os, const Request& req);

#endif