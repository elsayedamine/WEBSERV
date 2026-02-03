#ifndef PARSE_HPP
#define PARSE_HPP

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <Request.hpp>

#define RSIZE 8000
#define WSIZE 8000

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
		int state;
		int status;
		int cr;
	
		void parseMethod();
		void parseTarget();
		void parseVersion();
		void parseHeader();
		void parseBody();
	public:
		Parser();
		const Request &getRequest() const;
		int getStatus() const;
		void setStatus(::status s);
		void operator()(std::string data);
		int checkNL(size_t &i);
};

typedef void (Parser::*stateHandler)();

int validateMethod(const std::string meth);
int validateTarget(const std::string tar);
int validateVersion(const std::string ver);
int validateHeader(const std::pair<std::string, std::string> head, const std::multimap<std::string, std::string> map);
int validateBody(const std::string str);

#endif