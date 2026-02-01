#include <Directive.hpp>
#include <Configuration.hpp>
#include <Server.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

std::string check_configfile(int ac, char **av)
{
	std::string ConfigFile = "testing/webserv.conf";
	std::string filename;

	if (ac > 2)
		{ std::cerr << "Usage: " << av[0] << " [config_file.conf]" << std::endl; return ""; }
	else
	{
		filename = (ac == 2) ? av[1] : ConfigFile;
		size_t dotPos = filename.find_last_of('.');
		if (dotPos == std::string::npos || filename.substr(dotPos) != ".conf")
			{ std::cerr << "Error: File '" + filename + "' must have a .conf extension.\n"; return ""; }
		std::ifstream file(filename.c_str());
		if (!file.good())
			{ std::cerr << "Error: Cannot open file '" + filename + "'."; return ""; }
	}

	return filename;
}

int main(int ac, char **av)
{
	std::string filename; 
	if ((filename = check_configfile(ac, av)) == "")
		return EXIT_FAILURE;

	initSignals();
	Directive Dir;
	try { Dir =  Directive(filename.c_str()); }
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
