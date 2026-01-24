#include <Directive.hpp>
#include <Configuration.hpp>
#include <Server.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

int main(int ac, char **av)
{
	// check (extension)
	std::string ConfigFile = "testing/webserv.conf";
	if (ac == 2)
		ConfigFile = av[1];
	else if (ac > 2)
		{ std::cerr << "Usage: " << av[0] << " [config_file.conf]" << std::endl; return 1; }

	Directive Dir;
	try { Dir =  Directive(ConfigFile.c_str()); }
	catch (std::exception &e) { std::cerr << e.what() << std::endl; return 1; }

	Configuration Conf;
	try { Conf = Configuration(Dir); }
	catch (std::exception &e) { std::cerr << e.what() << std::endl; return 1; }

	Server Serv;
	try { Serv = Server(Conf); }
	catch (std::exception &e) { std::cerr << e.what() << std::endl; return 1; }
	Serv.run();
	return 0;
}
