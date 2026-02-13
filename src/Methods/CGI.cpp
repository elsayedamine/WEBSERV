#include <CGI.hpp>
#include <Request.hpp>
#include <Server.hpp>
#include <iostream>
#include <sstream>

const std::string &CGI::getBuffer() const { return buffer; }

void CGI::setBuffer(const std::string &buf) { buffer = buf; }

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
	char *buffer;

	buffer = getcwd(NULL, 0);
	if (request.getMethod() == "POST")
		variables.push_back("CONTENT_LENGTH=" + InttoString(request.getBody().size()));
	variables.push_back("REQUEST_METHOD=" + request.getMethod());
	variables.push_back("QUERY_STRING=" + ((request.getQuery().empty()) ? "" : request.getQuery()));
	variables.push_back("CONTENT_TYPE=" + request.getHeader("Content-Type"));
	variables.push_back("PATH_INFO=" + ((sep == std::string::npos) ? request.getTarget() : request.getTarget().substr(0, sep)));
	variables.push_back("SCRIPT_NAME=" + script);
	variables.push_back("SCRIPT_FILENAME=" + std::string(buffer) + ((script[0] != '/') ? "/" : "") + script);
	variables.push_back("SERVER_PROTOCOL=" + request.getVersion());
	variables.push_back("SERVER_SOFTWARE=" + std::string("webserv/1.0"));
	variables.push_back("GATEWAY_INTERFACE=" + std::string("CGI/1.1"));
	variables.push_back("REMOTE_ADDR=127.0.0.1");
	for (std::multimap<std::string, std::string>::const_iterator it = request.getHeaders().begin(); it != request.getHeaders().end(); ++it)
		variables.push_back("HTTP_" + processHeader((*it).first) + "=" + it->second);
	free(buffer);
}

void 	CGI::freeEnvp()
{
	if (!envp)
		return ;
	for (size_t i = 0; i < variables.size(); ++i)
		delete[] envp[i];
	delete[] envp;
	envp = NULL;
}

void	CGI::ConvertEnvp()
{
	envp = new char*[variables.size() + 1];
	size_t i = 0;
	try
	{
		for (;i < variables.size(); ++i)
		{
			envp[i] = new char[variables[i].size() + 1];
			std::strcpy(envp[i], variables[i].c_str());
		}
		envp[variables.size()] = NULL;
	}
	catch (...)
	{
		while (i > 0)
			delete[] envp[--i];
		delete[] envp;
		envp = NULL;
		throw std::runtime_error("Allocation failed!");
	}
}

void CGI::handleCGI(const Request &request, const std::string &script, const std::string &interpret)
{
	CreateVariables(request, script);
	ConvertEnvp();
	int pipe_in[2];
	int pipe_out[2];

	if (pipe(pipe_in) < 0 || pipe(pipe_out) < 0)
		{ freeEnvp(); return ; }
	pid_t pid = fork();
	if (pid < 0) {
		close(pipe_in[0]); close(pipe_in[1]);
		close(pipe_out[0]); close(pipe_out[1]);
		freeEnvp();
		return ;
	}
	else if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		size_t last_slash = script.find_last_of("/");
		std::string dir = (last_slash == std::string::npos) ? "." : script.substr(0, last_slash);
		std::string file = (last_slash == std::string::npos) ? script : script.substr(last_slash + 1);

		if (chdir(dir.c_str()) < 0) std::exit(1);
		dup2(pipe_in[0], STDIN_FILENO);
		dup2(pipe_out[1], STDOUT_FILENO);
		close(pipe_in[0]); close(pipe_in[1]);
		close(pipe_out[0]); close(pipe_out[1]);
		char *argv[] = {(char *)interpret.c_str(), (char *)file.c_str(), NULL};
		execve(argv[0], argv, envp);
		// std::cerr << "Failed to execute command: " << argv[0] << std::endl;
		// std::cerr << "Error: " << strerror(errno) << std::endl;
		std::exit(1); // the child is a snapshot of the living parent (peak jomla)
	}
	else
	{
		signal(SIGINT, handler);
		signal(SIGQUIT, SIG_IGN);
		close(pipe_in[0]); // cgi will write in pipe_in[1]
		close(pipe_out[1]); // cgi will read from pipe_out[0]

		// adding the pipe ends to the epoll()
		Server::setEvents(pipe_in[1], EPOLLOUT, EPOLL_CTL_ADD);
		Server::setEvents(pipe_out[0], EPOLLIN, EPOLL_CTL_ADD);

		in = pipe_in[1]; // write to child
		out = pipe_out[0]; // read from child
		this->pid = pid;

		freeEnvp();
	}
}

void CGI::parseHeader(Response &response) {
	std::string::size_type nl = buffer.find('\n');
	if (nl == std::string::npos) {
		state = CGI_PENDING;
		return;
	}
	std::string line = buffer.substr(0, nl);
	buffer.erase(0, nl + 1);
	if (!line.empty() && line[line.size() - 1] == '\r')
		line.erase(line.size() - 1);
	if (line.empty()) {
		state = CGI_BODY;
		return;
	}
	std::string::size_type colon = line.find(':');
	if (colon == std::string::npos || colon == 0) {
		state = CGI_FAIL;
		return;
	}
	std::string key = line.substr(0, colon);
	std::string value = line.substr(colon + 1);
	key = strtrim(key);
	value = strtrim(value);
	if (key.empty()) {
		state = CGI_FAIL;
		return;
	}
	if (key == "Status") {
		std::istringstream iss(value);
		int status = 0;
		iss >> status;
		if (status >= 100 && status <= 599)
			response.setStatus(status);
		state = CGI_HEADERS;
	}
	response.setHeader(key, value);
	state = CGI_HEADERS;
}

void CGI::parseBody(Response &response)
{
	if (buffer.empty()) {
		state = CGI_OVER;
		return;
	}
	response.setBody(buffer);
	buffer.clear();
}

int CGI::parse(Response &response) {
	if (CGI_PENDING)
		state = CGI_HEADERS;
	while (state != CGI_OVER && state != CGI_FAIL) {
		if (state == CGI_HEADERS)
			parseHeader(response);
		else if (state == CGI_BODY)
			parseBody(response);
	}
	return state;
}
