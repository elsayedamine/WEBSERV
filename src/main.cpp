#include <Directive.hpp>
#include <Configuration.hpp>
#include <Server.hpp>

void testServer(Configuration &config) {
	int fd[2];
	const char* http_request =
		"GET /users HTTP/1.1\r\n"
		"Host: example.com\r\n"
		"Content-Type: application/json\r\n"
		"Content-Length: 175\r\n"
		"\r\n"
		"{\r\n"
		"\t\"user\": {\r\n"
		"\t\t\"name\": \"Alice Johnson\",\r\n"
		"\t\t\"email\": \"alice.johnson@example.com\",\r\n"
		"\t\t\"age\": 28,\r\n"
		"\t\t\"preferences\": {\r\n"
		"\t\t\t\"theme\": \"dark\",\r\n"
		"\t\t\t\"notifications\": true\r\n"
		"\t\t}\r\n"
		"\t}\r\n"
		"}";

	pipe(fd);
	write(fd[1], http_request, strlen(http_request));
	close(fd[1]);
	handleConnection(fd[0], (config.getServers())[0]);
}

int main(int ac, char **av)
{
	Directive Dir;
	try { Dir =  Directive(av[ac -1]); }
	catch (std::exception &e) { std::cerr << e.what() << std::endl; return 1; }

	Configuration Conf;
	try { Conf = Configuration(Dir); }
	catch (std::exception &e) { std::cerr << e.what() << std::endl; return 1; }

	Server Serv;
	try { Serv = Server(Conf); }
	catch (std::exception &e) { std::cerr << e.what() << std::endl; return 1; }
	// Serv.run();
	testServer(Conf);
	return 0;
}
