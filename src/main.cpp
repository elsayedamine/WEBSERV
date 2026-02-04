#include <Directive.hpp>
#include <Configuration.hpp>
#include <Server.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>

int main(int ac, char **av)
{
	std::string filename; 
	if ((filename = check_configfile(ac, av)) == "")
		return EXIT_FAILURE;
	try
	{
		initSignals();
		Directive dir(filename.c_str());
		Configuration conf(dir);
		Server serv(conf);
		serv.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
