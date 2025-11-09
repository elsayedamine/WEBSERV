#include "Directive.hpp"
#include "Configuration.hpp"
int main(int ac, char **av)
{
	Directive direct;
	try { direct =  Directive(av[ac -1]); }
	catch (std::exception &e) { std::cerr << e.what() << std::endl; return 1; }
	// need to stop execution when error are here
	// Directive.PrintDirective(0);

	Config conf;
	try { conf = Config(direct); }
	catch (std::exception &e) { std::cerr << e.what() << std::endl; return 1; }

	// Server serv;
	// try { serv = Server(conf); }
	// catch (std::exception &e) { std::cerr << e.what() << std::endl; return 1; }

	return 0;
}
