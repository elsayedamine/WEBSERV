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

#include <Directive.hpp>
#include <ConfigBlock.hpp>

class Configuration
{
	private:
		std::vector<ConfigBlock> servers;
		void	resolve_config();
	public:
		Configuration() {}
		Configuration(const Directive& directive);
		const std::vector<ConfigBlock> & getServers() const { return this->servers; };
		e_error	err;
};

#endif
