#include "Configuration.hpp"

void fill_errors(std::map<Configuration::e_error, std::string> &error)
{
	error[Configuration::ERROR_UNKNOWN_KEY] = "ERROR_UNKNOWN_KEY";
	error[Configuration::ERROR_DUPLICATE_KEY] = "ERROR_DUPLICATE_KEY";
	error[Configuration::ERROR_INVALID_KEY] = "ERROR_INVALID_KEY";
	error[Configuration::ERROR_INVALID_SERVER] = "ERROR_INVALID_SERVER";
	error[Configuration::ERROR_INVALID_PORT] = "ERROR_INVALID_PORT";
	error[Configuration::ERROR_INVALID_HOST] = "ERROR_INVALID_HOST";
	error[Configuration::ERROR_INVALID_SERVER_NAME] = "ERROR_INVALID_SERVER_NAME";
	error[Configuration::ERROR_INVALID_INDEX] = "ERROR_INVALID_INDEX";
	error[Configuration::ERROR_INVALID_ALLOWED_METHODS] = "ERROR_INVALID_METHODS";
	error[Configuration::ERROR_INVALID_ROOT] = "ERROR_INVALID_ROOT";
	error[Configuration::ERROR_INVALID_ERROR_PAGE] = "ERROR_INVALID_ERROR_PAGE";
	error[Configuration::ERROR_INVALID_CLIENT_MAX_BODY_SIZE] = "ERROR_INVALID_CLIENT_MAX_BODY_SIZE";
	error[Configuration::ERROR_INVALID_LOCATION] = "ERROR_INVALID_LOCATION";
	error[Configuration::ERROR_INVALID_PREFIX] = "ERROR_INVALID_PREFIX";
	error[Configuration::ERROR_INVALID_REDIRECT] = "ERROR_INVALID_REDIRECT";
	error[Configuration::ERROR_INVALID_CGI_PATH] = "ERROR_INVALID_CGI_PATH";
	error[Configuration::ERROR_INVALID_AUTOINDEX] = "ERROR_INVALID_AUTOINDEX";
	error[Configuration::ERROR_INVALID_UPLOAD_PATH] = "ERROR_INVALID_UPLOAD_PATH";
}

void fill_keys(std::map<std::string, Configuration::DirectiveInfo> &server_keys)
{
	server_keys.insert(std::make_pair("listen", Configuration::DirectiveInfo(true, false, &Configuration::validate_listen)));
	server_keys.insert(std::make_pair("index", Configuration::DirectiveInfo(false, false, &Configuration::validate_index)));
	server_keys.insert(std::make_pair("server_name", Configuration::DirectiveInfo(false, false, &Configuration::validate_server_name)));
	server_keys.insert(std::make_pair("allowed_methods", Configuration::DirectiveInfo(false, false, &Configuration::validate_allowed_methods)));
	server_keys.insert(std::make_pair("root", Configuration::DirectiveInfo(false, false, &Configuration::validate_root)));
	server_keys.insert(std::make_pair("error_page", Configuration::DirectiveInfo(true, false, &Configuration::validate_error_page)));
	server_keys.insert(std::make_pair("location", Configuration::DirectiveInfo(true, true, &Configuration::validate_location)));
	server_keys.insert(std::make_pair("client_max_body_size", Configuration::DirectiveInfo(false, false, &Configuration::validate_clients)));
	server_keys.insert(std::make_pair("autoindex", Configuration::DirectiveInfo(false, false, &Configuration::validate_autoindex)));
	server_keys.insert(std::make_pair("upload_store", Configuration::DirectiveInfo(false, false, &Configuration::validate_upload_store)));
	server_keys.insert(std::make_pair("upload_enable", Configuration::DirectiveInfo(false, false, &Configuration::validate_upload_enable)));
	server_keys.insert(std::make_pair("return", Configuration::DirectiveInfo(false, false, &Configuration::validate_return)));
}

Configuration::e_error Configuration::ValidateServer(const Directive & server)
{
	if (server.getName() != "server" || server.getValues().size() != 0 || server.getChildren().size() < 1)
		return (Configuration::ERROR_INVALID_SERVER);

	std::size_t i = 0;
	while (i < server.getChildren().size())
	{
		const Directive &d = server.getChildren()[i];
		// somehow the directive is empty
		// std::cout << d.getName() << " amine ";
		const std::map<std::string, DirectiveInfo>::iterator &it =  server_keys.find(d.getName());
		if (it == server_keys.end() || (it->second.expectsChildren ^ !d.getChildren().empty()) || \
			(!it->second.allowsMultiple && it->second.appearance++))
			return Configuration::ERROR_INVALID_KEY;
		if ((this->*(it->second.Validator))(d))
			return static_cast<Configuration::e_error>(it->second.error);
		i++;
	}

	return Configuration::ERROR_NONE;
}

Configuration::e_error Configuration::PreValidation(const Directive &d)
{
	std::vector<Directive> servers = d.getChildren();

	err = Configuration::ERROR_NONE;
	if (servers.empty())
		return Configuration::ERROR_INVALID_SERVER;
	for (std::vector<Directive>::iterator it = servers.begin(); it != servers.end(); ++it)
		if ((err = ValidateServer(*it)) != Configuration::ERROR_NONE)
			return err;
	return Configuration::ERROR_NONE;
}

Configuration::Configuration(const Directive &d)
{
	fill_errors(error);
	fill_keys(server_keys);
	if ((err = PreValidation(d)) != Configuration::ERROR_NONE)
		throw std::runtime_error(error[err]);

	// for (size_t i = 0; i < d.getChildren().size(); i++)
		// servers.push_back(fill_server(d.getChildren()[i]));

	if ((err = PostValidation()) != Configuration::ERROR_NONE)
		throw std::runtime_error(error[err]); 
}


Configuration::e_error Configuration::PostValidation() {return Configuration::ERROR_NONE;}

ServerConfig fill_server(const Directive &server) {(void)server; return ServerConfig();}

