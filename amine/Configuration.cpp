#include "Configuration.hpp"

void	fill_errors(std::map<e_error, std::string> &error)
{
	error[ERROR_UNKNOWN_KEY] = "ERROR_UNKNOWN_KEY";
	error[ERROR_DUPLICATE_KEY] = "ERROR_DUPLICATE_KEY";
	error[ERROR_INVALID_KEY] = "ERROR_INVALID_KEY";
	error[ERROR_INVALID_SERVER] = "ERROR_INVALID_SERVER";
	error[ERROR_INVALID_PORT] = "ERROR_INVALID_PORT";
	error[ERROR_INVALID_HOST] = "ERROR_INVALID_HOST";
	error[ERROR_INVALID_SERVER_NAME] = "ERROR_INVALID_SERVER_NAME";
	error[ERROR_INVALID_INDEX] = "ERROR_INVALID_INDEX";
	error[ERROR_INVALID_ALLOWED_METHODS] = "ERROR_INVALID_METHODS";
	error[ERROR_INVALID_ROOT] = "ERROR_INVALID_ROOT";
	error[ERROR_INVALID_ERROR_PAGE] = "ERROR_INVALID_ERROR_PAGE";
	error[ERROR_INVALID_CLIENT_MAX_BODY_SIZE] = "ERROR_INVALID_CLIENT_MAX_BODY_SIZE";
	error[ERROR_INVALID_LOCATION] = "ERROR_INVALID_LOCATION";
	error[ERROR_INVALID_PREFIX] = "ERROR_INVALID_PREFIX";
	error[ERROR_INVALID_REDIRECT] = "ERROR_INVALID_REDIRECT";
	error[ERROR_INVALID_CGI_PATH] = "ERROR_INVALID_CGI_PATH";
	error[ERROR_INVALID_AUTOINDEX] = "ERROR_INVALID_AUTOINDEX";
	error[ERROR_INVALID_UPLOAD_PATH] = "ERROR_INVALID_UPLOAD_PATH";
	error[ERROR_INVALID_UPLOAD_STATE] = "ERROR_INVALID_UPLOAD_STATE";
	error[ERROR_INVALID_KEY_IN_LOCATION] = "ERROR_INVALID_KEY_IN_LOCATION";
}

void	fill_server_keys(std::map<std::string, Validators> &server_keys)
{
	server_keys["listen"] = &validate_listen;
	server_keys["index"] = &validate_index;
	server_keys["server_name"] = &validate_server_name;
	server_keys["allowed_methods"] = &validate_allowed_methods;
	server_keys["root"] = &validate_root;
	server_keys["error_page"] = &validate_error_page;
	server_keys["location"] = &validate_location;
	server_keys["client_max_body_size"] = &validate_clients;
	server_keys["autoindex"] = &validate_autoindex;
	server_keys["upload_store"] = &validate_upload_store;
	server_keys["upload_enable"] = &validate_upload_enable;
	server_keys["return"] = &validate_return;
}

void	fill_location_keys(std::map<std::string, Validators> &location_keys)
{
    location_keys["root"] = &validate_root;
    location_keys["index"] = &validate_index;
    location_keys["autoindex"] = &validate_autoindex;
    location_keys["upload_enable"] = &validate_upload_enable;
    location_keys["upload_store"] = &validate_upload_store;
    location_keys["return"] = &validate_return;
    location_keys["error_page"] = &validate_error_page;
    location_keys["client_max_body_size"] = &validate_clients;
    location_keys["allow_methods"] = &validate_allowed_methods;
	// location_keys["cgi_path"] = &validate_cgi_path;
}

Configuration::Configuration(const Directive &main)
{
	std::map<e_error, std::string> error;

	fill_errors(error);

	const std::vector<Directive>& server_directives = main.getChildren();
	for (size_t i = 0; i < server_directives.size(); ++i)
	{
		const Directive& server = server_directives[i];
		if (server.getName() != "server" || server.getValues().size() != 0 || server.getChildren().size() < 1)
			throw std::runtime_error(error[ERROR_INVALID_SERVER]);
		ConfigBlock server_conf(server);
		if (server_conf.err != ERROR_NONE)
			throw std::runtime_error(error[server_conf.err]);
		servers.push_back(server_conf);
	}
}
