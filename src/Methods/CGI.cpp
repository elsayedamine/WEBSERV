#include <CGI.hpp>
#include <Server.hpp>

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

void CGI::handleCGI(const Request &request, const std::string &script, const std::string &interpret, int fd)
{
	CreateVariables(request, script);
	ConvertEnvp();
	int pipe_in[2];
	int pipe_out[2];

	if (pipe(pipe_in) < 0 || pipe(pipe_out) < 0)
		{ freeEnvp(); return ; }
	pid_t pid = fork();
	if (pid < 0)
		{ freeEnvp(); return ; }
	if (pid == 0)
	{
		size_t last_slash = script.find_last_of("/");
		std::string dir = (last_slash == std::string::npos) ? "." : script.substr(0, last_slash);
		std::string file = (last_slash == std::string::npos) ? script : script.substr(last_slash + 1);

		if (chdir(dir.c_str()) < 0) exit(1);
		dup2(pipe_in[0], STDIN_FILENO);
		dup2(pipe_out[1], STDOUT_FILENO);
		close(pipe_in[0]); close(pipe_in[1]);
		close(pipe_out[0]); close(pipe_out[1]);
		char *argv[] = {(char *)interpret.c_str(), (char *)file.c_str(), NULL};
		execve(argv[0], argv, envp);
		exit(1);
	}
	else
	{
		close(pipe_in[0]);
		close(pipe_out[1]);

		fcntl(pipe_in[1], F_SETFL, O_NONBLOCK);
		fcntl(pipe_out[0], F_SETFL, O_NONBLOCK);

		// adding the pipe ends to the epoll()
		struct epoll_event ev;
		ev.events = EPOLLOUT;
		ev.data.fd = pipe_in[1];
		epoll_ctl(Server::epoll_fd, EPOLL_CTL_ADD, pipe_in[1], &ev);

		ev.events = EPOLLIN;
		ev.data.fd = pipe_out[0];
		epoll_ctl(Server::epoll_fd, EPOLL_CTL_ADD, pipe_out[0], &ev);

		Server::cgi[fd].in = pipe_out[0];
		Server::cgi[fd].out = pipe_in[1];
		Server::cgi[fd].pid = pid;
		Server::connect[pipe_out[0]] = fd;
		Server::connect[pipe_in[1]] = fd;

		freeEnvp();
	}
}