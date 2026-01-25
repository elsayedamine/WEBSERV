#ifndef CGI_HPP
#define CGI_HPP

#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>
#include <Methods.hpp>
#include <wait.h>
#include <poll.h>
#include <sys/epoll.h>

class CGI
{
	private:
		std::vector<std::string> variables;
		char **envp;
		// int pipe_in;
		// int pipe_out;
		// pid_t pid;
	public:
		CGI() /*: pipe_in(-1), pipe_out(-1)*/ {};
		~CGI() { freeEnvp(); };
		const std::vector<std::string> &getVariables() const { return this->variables; }

		std::string handleCGI(const Request &, const std::string &, const std::string &);
		void	CreateVariables(const Request &, const std::string &);
		void	ConvertEnvp();
		void 	freeEnvp();
};


#endif