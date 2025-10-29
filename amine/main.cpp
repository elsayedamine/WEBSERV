#include "main.hpp"

int main(int ac, char **av)
{
	str config_file = "test.web";
	if (ac == 2)
		config_file = av[1];

	Directive *t = Directive::AST_Constructor(config_file);
	if (t == 0)
		return std::cerr << "Error parsing configuration file" << std::endl, 1;

	// Configuration config(*t);
	// Server server(config);
	// server.start();

	delete t;
	return 0;
}
