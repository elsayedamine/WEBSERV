#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <vector>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>
#include <map>
#include <wait.h>
#include <poll.h>
#include <sys/epoll.h>
#include <Response.hpp>

class Request;

enum CGIparserState {
	CGI_OVER,
	CGI_PENDING,
	CGI_HEADERS,
	CGI_BODY,
	CGI_FAIL = -1
};

class CGI
{
	private:
		std::vector<std::string> variables;
		char **envp;
		std::string buffer;

		void parseHeader(Response &response);
		void parseBody(Response &response);
		int state;

	public:
		CGI() : envp(NULL), buffer(), state(CGI_PENDING), in(-1), out(-1), pid(-1), offset(0) {};
		~CGI() { /*freeEnvp();*/ };

		int in;
		int out;
		pid_t pid;
		size_t offset;

		// Getters
		const std::vector<std::string> &getVariables() const { return this->variables; }
		const std::string &getBuffer() const;
		int isReady() const;
		int getState() const { return state; }

		// Setters
		void setBuffer(const std::string &buf);
		void setReady(int r);
		void setState(int s) { state = s; }

		void	handleCGI(const Request &, const std::string &, const std::string &);
		void	CreateVariables(const Request &, const std::string &);
		void	ConvertEnvp();
		void 	freeEnvp();
		int		parse(Response &response);
};

#endif