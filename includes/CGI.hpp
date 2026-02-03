#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <vector>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>
#include <wait.h>
#include <poll.h>
#include <sys/epoll.h>

class Request;

class CGI
{
	private:
		std::vector<std::string> variables;
		char **envp;
		std::string bufCGI;
		int ready;

	public:
		CGI() : envp(NULL), ready(0), in(-1), out(-1), pid(-1) {};
		~CGI() { freeEnvp(); };

		int in;
		int out;
		pid_t pid;

		// Getters
		const std::vector<std::string> &getVariables() const { return this->variables; }
		const std::string &getBufCGI() const;
		int isReady() const;

		// Setters
		void setBufCGI(const std::string &buf);
		void setReady(int r);

		void	handleCGI(const Request &, const std::string &, const std::string &);
		void	CreateVariables(const Request &, const std::string &);
		void	ConvertEnvp();
		void 	freeEnvp();
};

#endif