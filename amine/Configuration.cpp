#include "Configuration.hpp"

void	fill_errors(std::map<Config::e_error, std::string> &error)
{
	error[Config::ERROR_UNKNOWN_KEY] = "ERROR_UNKNOWN_KEY";
	error[Config::ERROR_DUPLICATE_KEY] = "ERROR_DUPLICATE_KEY";
	error[Config::ERROR_INVALID_KEY] = "ERROR_INVALID_KEY";
	error[Config::ERROR_INVALID_SERVER] = "ERROR_INVALID_SERVER";
	error[Config::ERROR_INVALID_PORT] = "ERROR_INVALID_PORT";
	error[Config::ERROR_INVALID_HOST] = "ERROR_INVALID_HOST";
	error[Config::ERROR_INVALID_SERVER_NAME] = "ERROR_INVALID_SERVER_NAME";
	error[Config::ERROR_INVALID_INDEX] = "ERROR_INVALID_INDEX";
	error[Config::ERROR_INVALID_ALLOWED_METHODS] = "ERROR_INVALID_METHODS";
	error[Config::ERROR_INVALID_ROOT] = "ERROR_INVALID_ROOT";
	error[Config::ERROR_INVALID_ERROR_PAGE] = "ERROR_INVALID_ERROR_PAGE";
	error[Config::ERROR_INVALID_CLIENT_MAX_BODY_SIZE] = "ERROR_INVALID_CLIENT_MAX_BODY_SIZE";
	error[Config::ERROR_INVALID_LOCATION] = "ERROR_INVALID_LOCATION";
	error[Config::ERROR_INVALID_PREFIX] = "ERROR_INVALID_PREFIX";
	error[Config::ERROR_INVALID_REDIRECT] = "ERROR_INVALID_REDIRECT";
	error[Config::ERROR_INVALID_CGI_PATH] = "ERROR_INVALID_CGI_PATH";
	error[Config::ERROR_INVALID_AUTOINDEX] = "ERROR_INVALID_AUTOINDEX";
	error[Config::ERROR_INVALID_UPLOAD_PATH] = "ERROR_INVALID_UPLOAD_PATH";
}

void	fill_server_keys(std::map<std::string, Config::Validators> &server_keys)
{
	server_keys["listen"] = &Config::validate_listen;
	server_keys["index"] = &Config::validate_index;
	server_keys["server_name"] = &Config::validate_server_name;
	server_keys["allowed_methods"] = &Config::validate_allowed_methods;
	server_keys["root"] = &Config::validate_root;
	server_keys["error_page"] = &Config::validate_error_page;
	server_keys["location"] = &Config::validate_location;
	server_keys["client_max_body_size"] = &Config::validate_clients;
	server_keys["autoindex"] = &Config::validate_autoindex;
	server_keys["upload_store"] = &Config::validate_upload_store;
	server_keys["upload_enable"] = &Config::validate_upload_enable;
	server_keys["return"] = &Config::validate_return;
}

void	fill_location_keys(std::map<std::string, Config::Validators> &location_keys)
{
    location_keys["root"] = &Config::validate_root;
    location_keys["index"] = &Config::validate_index;
    location_keys["autoindex"] = &Config::validate_autoindex;
    location_keys["upload_enable"] = &Config::validate_upload_enable;
    location_keys["upload_store"] = &Config::validate_upload_store;
    location_keys["return"] = &Config::validate_return;
    location_keys["error_page"] = &Config::validate_error_page;
    location_keys["client_max_body_size"] = &Config::validate_clients;
    location_keys["allowed_methods"] = &Config::validate_allowed_methods;
	// location_keys["cgi_path"] = &Config::validate_cgi_path;
}

ConfigBlock Config::ValidateAndFillServer(const Directive &server)
{
	ConfigBlock serv;
	std::map<std::string, int> directive_counts;

	for (std::size_t i = 0; i < server.getChildren().size(); ++i) {
		const Directive &d = server.getChildren()[i];
		const std::string &name = d.getName();
		const std::map<std::string, Validators>::iterator & it = server_keys.find(name);
		if (it == server_keys.end()) {
			err = ERROR_UNKNOWN_KEY;
			serv.valid = false;
			return serv;
		}
		int &count = directive_counts[name];
		if (name != "location" && name != "error_page" && count > 0) {
			err = ERROR_DUPLICATE_KEY;
			serv.valid = false;
			return serv;
		}
		count++;
		ConfigBlock tmp = (this->*(it->second))(d);
		// if (!tmp.locations.empty()) serv.locations.insert(serv.locations.end(), tmp.locations.begin(), tmp.locations.end());
		if (!tmp.valid) return serv.valid = false, tmp;
		if (!tmp.error_page.empty()) serv.error_page = tmp.error_page;
		if (tmp.port) serv.port = tmp.port;
		if (!tmp.host.empty()) serv.host = tmp.host;
		if (!tmp.root.empty()) serv.root = tmp.root;
		if (!tmp.client_max_body_size.empty()) ;
		if (!tmp.upload_path.empty()) ;
		if (!tmp.methods.empty()) ;
		if (!tmp.redirect.second.empty()) ;
		if (!tmp.server_name.empty()) serv.server_name = tmp.server_name;
		if (!tmp.index.empty()) serv.index = tmp.index;
		if (tmp.autoindex != false && !tmp.autoindex != true) serv.autoindex = tmp.autoindex; 
		//dont forget to overload the operator= for these ifs
		// location and error page
	}
	return serv;
}

Config::Config(const Directive &main)
{
	fill_errors(error);
	fill_server_keys(server_keys);
	fill_location_keys(location_keys);

	const std::vector<Directive>& server_directives = main.getChildren();
	for (size_t i = 0; i < server_directives.size(); ++i)
	{
		const Directive& server = server_directives[i];
		if (server.getName() != "server" || server.getValues().size() != 0 || server.getChildren().size() < 1)
			throw std::runtime_error(error[Config::ERROR_INVALID_SERVER]);
		ConfigBlock server_conf = ValidateAndFillServer(server);
		if (server_conf.valid == false)
			throw std::runtime_error(error[err]);
		servers.push_back(server_conf);
	}
}
