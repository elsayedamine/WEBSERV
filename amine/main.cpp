#include "Directive.hpp"
#include "Configuration.hpp"

int main(int ac, char **av)
{
	Directive direct;
	try { direct =  Directive(av[ac -1]); }
	catch (std::exception &e) { std::cerr << e.what() << std::endl; return 1; }

	Configuration conf;
	try { conf = Configuration(direct); }
	catch (std::exception &e) { std::cerr << e.what() << std::endl; return 1; }

	// Server serv;
	// try { serv = Server(conf); }
	// catch (std::exception &e) { std::cerr << e.what() << std::endl; return 1; }
	// serv.run();
	return 0;
}
