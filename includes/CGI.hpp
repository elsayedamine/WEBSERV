#ifndef CGI_HPP
#define CGI_HPP

#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>
#include <Methods.hpp>
#include <wait.h>

class CGI
{
	private:
		std::vector<std::string> variables;
		char **envp;
	public:
		CGI() {};
		~CGI() { freeEnvp(); };

		const std::vector<std::string> &getVariables() const { return this->variables; }
		std::string handleCGI(const Request &request, const std::string &script, const std::string &interpret);
		void	CreateVariables(const Request &request, const std::string &script);
		void	ConvertEnvp();
		void 	freeEnvp();
};


#endif