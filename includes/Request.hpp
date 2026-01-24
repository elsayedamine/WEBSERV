#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <vector>
#include <iostream>
#include <map>
#include <ConfigBlock.hpp>
#include <Response.hpp>

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
		int headerCount;

		ConfigBlock server;

	public:
		Request();

		void setHeader(const std::string &key, const std::string &value);
		void setBody(const std::string &b);
		void setServer(const ConfigBlock &server);
		void setMethod(const std::string &m);
		void setTarget(const std::string &t);
		void setVersion(const std::string &v);

		const std::string &getMethod() const;
		Method getMethodEnum() const;
		const std::string &getTarget() const;
		const std::string &getVersion() const;
		const std::multimap<std::string, std::string> &getHeaders() const;
		std::string getHeader(const std::string &key) const;
		const std::string &getBody() const;

		std::vector<ConfigBlock>::const_iterator getCandidate(const std::vector<ConfigBlock> &candidates) const;
		Response processRequest();
		int validateRequest() const;
		Response handleRequest(const std::vector<ConfigBlock> &locations) const;
};

typedef std::multimap<std::string, std::string>::const_iterator mmap_it;
typedef std::map<std::string, std::string>::const_iterator map_it;

#endif