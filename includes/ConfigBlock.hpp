#ifndef CONFIGBLOCK_HPP
# define CONFIGBLOCK_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cctype>
#include <map>
#include <cstdlib>

class Directive;

enum e_error {
	ERROR_NONE, ERROR_UNKNOWN_KEY,
	ERROR_DUPLICATE_KEY, ERROR_INVALID_KEY,
	ERROR_INVALID_SERVER, ERROR_INVALID_PORT,
	ERROR_INVALID_HOST,	ERROR_INVALID_SERVER_NAME,
	ERROR_INVALID_INDEX, ERROR_INVALID_ALLOWED_METHODS,
	ERROR_INVALID_ROOT, ERROR_INVALID_ERROR_PAGE,
	ERROR_INVALID_CLIENT_MAX_BODY_SIZE, ERROR_INVALID_LOCATION,
	ERROR_INVALID_PREFIX, ERROR_INVALID_RETURN,
	ERROR_INVALID_CGI, ERROR_INVALID_AUTOINDEX,
	ERROR_INVALID_UPLOAD_STATE,
	ERROR_INVALID_KEY_IN_LOCATION,
	ERROR_MISSING_LISTEN, ERROR_MISSING_SERVER_NAME,
	ERROR_MISSING_ROOT,
};

struct ConfigBlock
{
	e_error	err;
	// --- Server-Only ---
	unsigned short	port;
	std::string		host;
	std::vector<std::string> server_name;
	// --- Location-Only ---
	std::string		prefix;
	std::map<std::string, std::string> cgi;
	// --- Shared Fields ---
	int				autoindex;
	int				upload_enable;
	std::string		root;
	long		client_max_body_size;
	std::map<int,std::string> error_page;
	std::pair<int,std::string> ret;
	std::vector<std::string>	index;
	std::vector<std::string>	methods;
	// --- Mandatory members --- //
	bool has_listen;
	bool has_root;
	bool has_server_name;
	// --- For Nesting ---
	std::vector<ConfigBlock> locations; 

	ConfigBlock() : err((e_error)0), port(0), autoindex(-1), upload_enable(-1), client_max_body_size(-1), \
					has_listen(false), has_root(false), has_server_name(false) {}
	ConfigBlock(const Directive &);
	void applyDefaultsToLocations();
	void mergeForm(const ConfigBlock &other);
};

ConfigBlock	validate_listen(const Directive &);
ConfigBlock	validate_index(const Directive &);
ConfigBlock	validate_server_name(const Directive &);
ConfigBlock	validate_allowed_methods(const Directive &);
ConfigBlock	validate_root(const Directive &);
ConfigBlock	validate_error_page(const Directive &);
ConfigBlock	validate_location(const Directive &);
ConfigBlock	validate_clients(const Directive &);
ConfigBlock	validate_autoindex(const Directive &);
ConfigBlock	validate_upload_enable(const Directive &);
ConfigBlock	validate_return(const Directive &);
ConfigBlock	validate_cgi(const Directive &);

typedef     ConfigBlock (*Validators)(const Directive &);
void		fill_location_keys(std::map<std::string, Validators> &location_keys);
void		fill_server_keys(std::map<std::string, Validators> &server_keys);

#endif