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

class CGI
{
	private:
		std::vector<std::string> variables;
		char **envp;
		std::string buffer;

		int parseHeader(Response &response);
		void parseBody(Response &response);

	public:
		CGI() : envp(NULL), in(-1), out(-1), pid(-1), offset(0) {};
		~CGI() { /*freeEnvp();*/ };

		int in;
		int out;
		pid_t pid;
		size_t offset;

		// Getters
		const std::vector<std::string> &getVariables() const { return this->variables; }
		const std::string &getBuffer() const;
		int isReady() const;

		// Setters
		void setBuffer(const std::string &buf);
		void setReady(int r);

		void	handleCGI(const Request &, const std::string &, const std::string &);
		void	CreateVariables(const Request &, const std::string &);
		void	ConvertEnvp();
		void 	freeEnvp();
		bool	parse();
};

#endif