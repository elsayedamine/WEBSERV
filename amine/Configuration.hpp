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

struct ConfigBlock
{
	int				valid;
	// --- Server-Only ---
	unsigned short	port;
	std::string		host;
	std::vector<std::string> server_name;
	// --- Location-Only ---
	std::string		prefix;
	std::vector<std::string> cgi_path;
	// --- Shared Fields ---
	int				autoindex;
	int				upload_enable;
	std::string		root;
	std::string		upload_path;
	std::string		client_max_body_size;
	std::map<int,std::string> error_page;
	std::pair<int,std::string> redirect;
	std::vector<std::string>	index;
	std::vector<std::string>	methods;
	// --- For Nesting ---
	std::vector<ConfigBlock> locations; 

	ConfigBlock() : valid(0), port(0), autoindex(-1), upload_enable(-1) {}
};

class Config
{
	public:
		typedef ConfigBlock (Config::*Validators)(const Directive &);

		Config(const Directive& directive);
		const	std::vector<ConfigBlock>& getServers() const { return servers; }
		const	std::map<std::string, Validators>& getServerKeys() const { return server_keys; }

		enum e_error {
			ERROR_NONE,
			ERROR_UNKNOWN_KEY,
			ERROR_DUPLICATE_KEY,
			ERROR_INVALID_KEY,
			ERROR_INVALID_SERVER,
			ERROR_INVALID_PORT,
			ERROR_INVALID_HOST,
			ERROR_INVALID_SERVER_NAME,
			ERROR_INVALID_INDEX,
			ERROR_INVALID_ALLOWED_METHODS,
			ERROR_INVALID_ROOT,
			ERROR_INVALID_ERROR_PAGE,
			ERROR_INVALID_CLIENT_MAX_BODY_SIZE,
			ERROR_INVALID_LOCATION,
			ERROR_INVALID_PREFIX,
			ERROR_INVALID_REDIRECT,
			ERROR_INVALID_CGI_PATH,
			ERROR_INVALID_AUTOINDEX,
			ERROR_INVALID_UPLOAD_PATH
		};
		std::map<e_error, std::string> error;
		e_error	err;

		ConfigBlock	validate_listen(const Directive &);
		ConfigBlock	validate_index(const Directive &);
		ConfigBlock	validate_server_name(const Directive &);
		ConfigBlock	validate_allowed_methods(const Directive &);
		ConfigBlock	validate_root(const Directive &);
		ConfigBlock	validate_error_page(const Directive &);
		ConfigBlock	validate_location(const Directive &);
		ConfigBlock	validate_clients(const Directive &);
		ConfigBlock	validate_autoindex(const Directive &);
		ConfigBlock	validate_upload_store(const Directive &);
		ConfigBlock	validate_upload_enable(const Directive &);
		ConfigBlock	validate_return(const Directive &);
		ConfigBlock	validate_cgi_path(const Directive &);

	private:
		ConfigBlock	ValidateAndFillServer(const Directive & server);
		std::map<std::string, Validators> server_keys;
		std::map<std::string, Validators> location_keys;
		std::vector<ConfigBlock> servers;

};



#endif
