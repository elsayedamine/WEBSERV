#include "Configuration.hpp"

void fill_errors(std::map<Configuration::e_error, std::string> &error)
{
	error[Configuration::ERROR_UNKNOWN_KEY] = "ERROR_UNKNOWN_KEY";
	error[Configuration::ERROR_DUPLICATE_KEY] = "ERROR_DUPLICATE_KEY";
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

Configuration::e_error validate_server(const Directive & server)
{
	static std::string server_keys[] = {"listen", "index", "server_name", "allowed_methods", \
		"root", "error_page", "client_max_body_size", "location", "autoindex", \
		"upload_store", "upload_enable"	"return"};
	if (server.getName() != "server")
		return (Configuration::ERROR_INVALID_SERVER);
	for (std::size_t i = 0; i < server.getChildren().size(); ++i) {
		for (int i = 0; i < 12; ++i) {
			if (server.getChildren()[i].getName() == server_keys[i])
				break; }
		if (i == 12) return Configuration::ERROR_UNKNOWN_KEY;
		//loop done
	}
	// loop over the directive children to spot unknown key
	// then i should go to each directive(server_key) and validate their values 
	return Configuration::ERROR_NONE;
}


Configuration::e_error Configuration::PostValidation() {return Configuration::ERROR_NONE;}

ServerConfig fill_server(const Directive &server) {(void)server; return ServerConfig();}


Configuration::e_error Configuration::PreValidation(const Directive &d)
{
	std::vector<Directive> servers = d.getChildren();

	err = Configuration::ERROR_NONE;
	if (servers.empty())
		return Configuration::ERROR_INVALID_SERVER;
	for (std::vector<Directive>::iterator it = servers.begin(); it != servers.end(); ++it)
		if ((err = validate_server(*it)) != Configuration::ERROR_NONE)
			return err;
	return Configuration::ERROR_NONE;
}

Configuration::Configuration(const Directive &d)
{
	fill_errors(error);
	if ((err = PreValidation(d)) != Configuration::ERROR_NONE)
		throw std::runtime_error(error[err]);

	for (size_t i = 0; i < d.getChildren().size(); i++)
		servers.push_back(fill_server(d.getChildren()[i]));

	if ((err = PostValidation()) != Configuration::ERROR_NONE)
		throw std::runtime_error(error[err]); 
}
