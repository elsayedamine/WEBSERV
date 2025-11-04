#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cctype>
#include <map>

#include "Directive.hpp"

struct LocationConfig
{
	std::string		prefix;
	std::string		root;
	std::string		autoindex;
	std::string		upload_path;
	std::string		client_max_body_size;
	std::vector<std::string>	cgi_path;
	std::vector<std::string>	error_page;
	std::vector<std::string>	redirect;
	std::vector<std::string>	index;
	std::vector<std::string>	allowed_methods;
};

struct ServerConfig
{
	int				valid;
	unsigned short	port;
	unsigned int	host;
	std::string		root;
	std::string		client_max_body_size;
	std::string		upload_path;
	std::vector<std::string> allowed_methods;
	std::vector<std::string> redirect;
	std::vector<std::string> server_name;
	std::vector<std::string> index;
	std::map<int,std::string> error_page;

	std::vector<LocationConfig> locations;
};

class Configuration
{
	public:
		typedef ServerConfig (Configuration::*isValidDirective) (const Directive&);
		struct DirectiveInfo
		{
			bool	allowsMultiple;
			bool	expectsChildren;
			int		appearance; // needs to be removed and replaced by a better idea
			int		values;
			int		error;
			isValidDirective Validator;
			DirectiveInfo(bool multiple, bool children, isValidDirective fct)
				: allowsMultiple(multiple), expectsChildren(children), appearance(0), values(0), error(0), Validator(fct) {}
		};

		Configuration(const Directive& directive);
		const	std::vector<ServerConfig>& getServers() const { return servers; }
		const	std::map<std::string, DirectiveInfo>& getServerKeys() const { return server_keys; }

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
		ServerConfig	validate_listen(const Directive &);
		ServerConfig	validate_index(const Directive &listen);
		ServerConfig	validate_server_name(const Directive &listen);
		ServerConfig	validate_allowed_methods(const Directive &listen);
		ServerConfig	validate_root(const Directive &listen);
		ServerConfig	validate_error_page(const Directive &listen);
		ServerConfig	validate_location(const Directive &listen);
		ServerConfig	validate_clients(const Directive &listen);
		ServerConfig	validate_autoindex(const Directive &listen);
		ServerConfig	validate_upload_store(const Directive &listen);
		ServerConfig	validate_upload_enable(const Directive &listen);
		ServerConfig	validate_return(const Directive &listen);

	private:
		std::vector<ServerConfig> servers;
		e_error PostValidation();
		ServerConfig	ValidateAndFillServer(const Directive & server);
		std::map<std::string, DirectiveInfo> server_keys;

};



#endif