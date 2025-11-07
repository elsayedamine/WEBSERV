#include "Directive.hpp"
#include "Configuration.hpp"
int main(int ac, char **av)
{
	Directive Directive(av[ac -1]);
	// need to stop execution when error are here
	// Directive.PrintDirective(0);

	try { Config config(Directive); }
	catch (std::exception &e) { std::cerr << e.what() <<std::endl; }
	// Server server(config);
	// server.start();

	return 0;
}
