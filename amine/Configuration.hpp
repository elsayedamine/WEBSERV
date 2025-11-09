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

class Config
{
	private:
		std::vector<ConfigBlock> servers;
	public:
		Config() {}
		Config(const Directive& directive);
		e_error	err;
};

#endif
