#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cctype>
#include <map>
#include <cstdlib>

#include "Directive.hpp"
#include "ConfigBlock.hpp"

class Configuration
{
	private:
		std::vector<ConfigBlock> servers;
	public:
		Configuration() {}
		Configuration(const Directive& directive);
		e_error	err;
};

#endif
