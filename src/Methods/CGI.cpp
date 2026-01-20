#include <CGI.hpp>

std::string InttoString(int x)
{
	std::ostringstream oss;
	oss << x;
	std::string s = oss.str();
	return s;
}

std::string processHeader(const std::string &header)
{
	std::size_t i = -1;
	std::string uppered = "";
	while (header[++i])
		uppered += ((header[i] == '-') ? '_' : (char)std::toupper(header[i]));
	return uppered;
}

void	CGI::CreateVariables(const Request &request, const std::string &script)
{
	std::size_t sep = request.getTarget().find('?');
	char buffer[10000];

	if (request.getMethod() == "POST")
		variables.push_back("CONTENT_LENGTH=" + InttoString(request.getBody().size()));
	variables.push_back("REQUEST_METHOD=" + request.getMethod());
	variables.push_back("QUERY_STRING=" + ((sep == string::npos) ? "" : request.getTarget().substr(sep + 1)));
	variables.push_back("CONTENT_TYPE=" + request.getHeader("Content-Type"));
	variables.push_back("PATH_INFO=" + ((sep == string::npos) ? request.getTarget() : request.getTarget().substr(0, sep)));
	variables.push_back("SCRIPT_NAME=" + script);
	variables.push_back("SCRIPT_FILENAME=" + std::string(getcwd(buffer, 10000)) + ((script[0] != '/') ? "/" : "") + script);
	variables.push_back("SERVER_PROTOCOL=" + request.getVersion());
	variables.push_back("SERVER_SOFTWARE=" + std::string("webserv/1.0"));
	variables.push_back("GATEWAY_INTERFACE=" + std::string("CGI/1.1"));
	variables.push_back("REMOTE_ADDR=127.0.0.1");
	for (std::multimap<std::string, std::string>::const_iterator it = request.getHeaders().begin(); it != request.getHeaders().end(); ++it)
		variables.push_back("HTTP_" + processHeader((*it).first) + "=" + it->second);
}

void 	CGI::freeEnvp()
{
	for (size_t i = 0; i < variables.size(); ++i)
		delete[] envp[i];
	delete[] envp;
}

void	CGI::ConvertEnvp()
{
	envp = new char*[variables.size() + 1];
	try
	{
		for (size_t i = 0; i < variables.size(); ++i)
		{
			envp[i] = new char[variables[i].size() + 1];
			std::strcpy(envp[i], variables[i].c_str());
		}
		envp[variables.size()] = NULL;
	}
	catch (...)
	{
		freeEnvp();
		throw std::runtime_error("Allocation failed!");
	}
}

std::string CGI::handleCGI(const Request &request, const std::string &script, const std::string &interpret)
{
	CreateVariables(request, script);
	ConvertEnvp();
	int pipe_in[2];
	int pipe_out[2];

	if (pipe(pipe_in) < 0 || pipe(pipe_out) < 0)
		{ freeEnvp(); return std::string(""); }
	pid_t pid = fork();
	if (pid < 0)
		{ freeEnvp(); return std::string(""); }
	if (pid == 0)
	{
		dup2(pipe_in[0], STDIN_FILENO);
		dup2(pipe_out[1], STDOUT_FILENO);
		close(pipe_in[0]); close(pipe_in[1]);
		close(pipe_out[0]); close(pipe_out[1]);
		char *argv[] = {(char *)interpret.c_str(), (char *)script.c_str(), NULL};
		execve(argv[0], argv, envp);
		exit(1);
	}
	else
	{
		close(pipe_in[0]);
		close(pipe_out[1]);
		write(pipe_in[1], request.getBody().c_str(), request.getBody().size());
		close(pipe_in[1]);
		char buffer[10000];
		std::string result;
		ssize_t bytes;
		while ((bytes = read(pipe_out[0], buffer, sizeof(buffer))) > 0)
			result.append(buffer, bytes);
		// check this needs to be added to the multiplexer; any read or write should pass by epoll();
		// this is completely wrong for the subject
		// also the timeout needs to be handled outside
		// i need to add the event struct to the class

		// TL;DR I/O operations maghadarch hnaya
		close(pipe_out[0]);
		waitpid(pid, NULL, 0);
		freeEnvp();
		return result;
	}
}
