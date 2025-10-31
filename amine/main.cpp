#include "main.hpp"

int main(int ac, char **av)
{
	std::string config_file = "test.web";
	if (ac == 2)
		config_file = av[1];

	Directive Directive(config_file.c_str());
	Directive.PrintDirective(0);

	// Configuration config(*t);
	// Server server(config);
	// server.start();

	return 0;
}
