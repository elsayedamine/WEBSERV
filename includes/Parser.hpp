#ifndef PARSE_HPP
#define PARSE_HPP

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <Request.hpp>

#define RSIZE 8000	
#define WSIZE 8000
#define MAX_TARGET_SIZE 1000

enum errors {
	URI_TOO_LONG = 414,
	BODY_TOO_LONG = 413,
	LENGTH_REQUIRED = 411,
	UNSUPPORTED_HTTP = 505
};

enum state {
	STATE_METHOD,
	STATE_TARGET,
	STATE_VERSION,
	STATE_HEADERS,
	STATE_BODY
};

enum status {
	PARSE_FAIL,
	PARSE_SUCCESS,
	PARSE_CURRENT,
	PARSE_PENDING,
	PARSE_OVER
};

class Parser {
	private:
		Request request;
		std::string current;
		std::vector<ConfigBlock> servers;
		int state;
		int status;
		int cr;
		int err;
	
		void parseMethod();
		void parseTarget();
		void parseQuery();
		void parseVersion();
		void parseHeader();
		void parseBody();
	public:
		Parser();
		Parser(std::vector<ConfigBlock> servers);
		const Request &getRequest() const;
		int getStatus() const;
		int getErr() const;
		void setStatus(::status s);
		void operator()(std::string data);
		int checkNL(size_t &i);
		int validateMethod(const std::string meth);
		int validateTarget(const std::string tar);
		int validateQuery(const std::string que);
		int validateVersion(const std::string ver);
		int validateHeader(const std::pair<std::string, std::string> head, const std::multimap<std::string, std::string> map);
		int validateBody(const std::string str);
};

typedef void (Parser::*stateHandler)();


#endif