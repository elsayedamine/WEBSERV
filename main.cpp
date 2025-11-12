#include <Directive.hpp>
#include <Configuration.hpp>
#include <Server.hpp>

int main(int ac, char **av)
{
	Directive Dir;
	try { Dir =  Directive(av[ac -1]); }
	catch (std::exception &e) { std::cerr << e.what() << std::endl; return 1; }

	Configuration Conf;
	try { Conf = Configuration(Dir); }
	catch (std::exception &e) { std::cerr << e.what() << std::endl; return 1; }

	Server serv;
	try { serv = Server(Conf); }
	catch (std::exception &e) { std::cerr << e.what() << std::endl; return 1; }
	serv.run();
	return 0;
}
