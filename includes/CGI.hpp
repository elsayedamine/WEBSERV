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

struct CGIHandle
{
	int in;
	int out;
	pid_t pid;
};

class CGI
{
	private:
		std::vector<std::string> variables;
		char **envp;

	public:
		CGI(){};
		~CGI() { freeEnvp(); };

		// Getters
		const std::vector<std::string> &getVariables() const { return this->variables; }

		void	handleCGI(const Request &, const std::string &, const std::string &, int fd);
		void	CreateVariables(const Request &, const std::string &);
		void	ConvertEnvp();
		void 	freeEnvp();
};

#endif