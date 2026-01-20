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
	// now i create l pipe...ndwzha l fork....take the output and shit
}
