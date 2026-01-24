#ifndef PARSE_HPP
#define PARSE_HPP

#include <iostream>
#include <string>
#include <sys/socket.h>
// #include <Request.hpp>

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
	STATE_FAIL,
	STATE_SUCCESS,
	STATE_CURRENT,
	STATE_OVER
};

class Request;

class Parse {
	private:
		int fd;
		std::string current;
		int state;
		success status;
		Request &request;
	
	public:
		Parse(Request &req);
		// void loopParse(std::string data);
		void operator()(std::string data);
		void parseMethod();
		void parseTarget();
		void parseVersion();
		void parseHeaders();
		void parseBody();
};

#endif