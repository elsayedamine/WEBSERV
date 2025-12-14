#include <Directive.hpp>
#include <Configuration.hpp>
#include <Server.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

void testServer(Configuration &config) {
	int fd[2];

	// Read the raw HTTP request from a file (relative to the working directory).
	const char *request_file = "testing/requests/1";
	std::ifstream in(request_file, std::ios::in | std::ios::binary);
	if (!in.is_open()) {
		std::cerr << "testServer: failed to open request file '" << request_file << "'\n";
		return;
	}
	std::ostringstream buf;
	buf << in.rdbuf();
	std::string http_request = buf.str();

	if (pipe(fd) == -1) {
		std::cerr << "testServer: pipe() failed\n";
		return;
	}
	ssize_t to_write = static_cast<ssize_t>(http_request.size());
	ssize_t written = write(fd[1], http_request.c_str(), to_write);
	if (written != to_write) {
		std::cerr << "testServer: write() wrote " << written << " of " << to_write << " bytes\n";
	}
	close(fd[1]);
	handleConnection(fd[0], config.getServers());
}

int main(int ac, char **av)
{
	// open file correctly
	Directive Dir;
	try { Dir =  Directive(av[ac -1]); }
	catch (std::exception &e) { std::cerr << e.what() << std::endl; return 1; }

	Configuration Conf;
	try { Conf = Configuration(Dir); }
	catch (std::exception &e) { std::cerr << e.what() << std::endl; return 1; }

	Server Serv;
	try { Serv = Server(Conf); }
	catch (std::exception &e) { std::cerr << e.what() << std::endl; return 1; }
	Serv.run();
	// testServer(Conf);
	return 0;
}
