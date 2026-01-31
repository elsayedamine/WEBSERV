#ifndef PARSE_HPP
#define PARSE_HPP

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <Request.hpp>

#define RSIZE 8000
#define WSIZE 8000

enum states {
	STATE_METHOD,
	STATE_TARGET,
	STATE_VERSION,
	STATE_HEADERS,
	STATE_BODY
};

enum success {
	PARSE_FAIL,
	PARSE_SUCCESS,
	PARSE_CURRENT,
	PARSE_OVER
};

class Parse {
	private:
		Request request;
		int fd;
		std::string current;
		int state;
		success status;
		int cr;
	
	public:
		Parse();
		void operator()(std::string data);
		void parseMethod();
		void parseTarget();
		void parseVersion();
		void parseHeader();
		void parseBody();
		int checkNL(size_t &i);
};

typedef void (Parse::*stateHandler)();

#endif