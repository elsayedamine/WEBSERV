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

void	fill_keys(std::map<std::string, Config::DirectiveInfo> &server_keys)
{
	server_keys.insert(std::make_pair("listen", Config::DirectiveInfo(false, false, &Config::validate_listen)));
	server_keys.insert(std::make_pair("index", Config::DirectiveInfo(false, false, &Config::validate_index)));
	server_keys.insert(std::make_pair("server_name", Config::DirectiveInfo(false, false, &Config::validate_server_name)));
	server_keys.insert(std::make_pair("allowed_methods", Config::DirectiveInfo(false, false, &Config::validate_allowed_methods)));
	server_keys.insert(std::make_pair("root", Config::DirectiveInfo(false, false, &Config::validate_root)));
	server_keys.insert(std::make_pair("error_page", Config::DirectiveInfo(true, false, &Config::validate_error_page)));
	server_keys.insert(std::make_pair("location", Config::DirectiveInfo(true, true, &Config::validate_location)));
	server_keys.insert(std::make_pair("client_max_body_size", Config::DirectiveInfo(false, false, &Config::validate_clients)));
	server_keys.insert(std::make_pair("autoindex", Config::DirectiveInfo(false, false, &Config::validate_autoindex)));
	server_keys.insert(std::make_pair("upload_store", Config::DirectiveInfo(false, false, &Config::validate_upload_store)));
	server_keys.insert(std::make_pair("upload_enable", Config::DirectiveInfo(false, false, &Config::validate_upload_enable)));
	server_keys.insert(std::make_pair("return", Config::DirectiveInfo(false, false, &Config::validate_return)));
}

void	fill_ref(std::map<std::string, std::string> & ref)
{
	ref["listen"] = "";
	ref["server_name"] = "";
	ref["root"] = "";
	ref["autoindex"] = "";
	ref["error_page"] = "";
	ref["client_max_body_size"] = "";
	ref["allowed_methods"] = "";
	ref["location"] = "";
	ref["host"] = "";
	ref["return"] = "";
	ref["upload_enable"] = "";
	ref["upload_store"] = "";
}

ServerConfig Config::ValidateAndFillServer(const Directive &server)
{
	ServerConfig serv;

	for (std::size_t i = 0; i < server.getChildren().size(); ++i) {
		const Directive &d = server.getChildren()[i];
		const std::map<std::string, DirectiveInfo>::iterator & it = server_keys.find(d.getName());
		if (it == server_keys.end()) {
			err = ERROR_UNKNOWN_KEY;
			serv.valid = false;
			return serv;
		}
		DirectiveInfo &info = it->second;
		if ((info.expectsChildren != !d.getChildren().empty()) || (!info.allowsMultiple && info.values > 0)) {
			err = (info.expectsChildren != !d.getChildren().empty()) ? ERROR_INVALID_KEY : ERROR_DUPLICATE_KEY;
			serv.valid = false;
			return serv;
		}
		info.values++;
		ServerConfig tmp = (this->*(info.Validator))(d);
		if (!tmp.valid) return serv.valid = false, tmp;
		if (!tmp.root.empty()) serv.root = tmp.root;
		if (!tmp.server_name.empty()) serv.server_name = tmp.server_name;
		if (tmp.port) serv.port = tmp.port;
		if (tmp.host) serv.host = tmp.host;
		if (!tmp.index.empty()) serv.index = tmp.index;
		if (!tmp.error_page.empty()) serv.error_page = tmp.error_page;
		if (!tmp.locations.empty()) serv.locations.insert(serv.locations.end(), tmp.locations.begin(), tmp.locations.end());
	}
	return serv;
}

Config::Config(const Directive &main)
{
	fill_errors(error);
	fill_keys(server_keys);
	// fill_ref(ref);
	const std::vector<Directive>& server_directives = main.getChildren();

	for (size_t i = 0; i < server_directives.size(); ++i)
	{
		const Directive& server = server_directives[i];
		if (server.getName() != "server" || server.getValues().size() != 0 || server.getChildren().size() < 1)
			throw std::runtime_error(error[Config::ERROR_INVALID_SERVER]);
		ServerConfig server_conf = ValidateAndFillServer(server);
		if (server_conf.valid == false)
			throw std::runtime_error(error[Config::ERROR_UNKNOWN_KEY]);
		servers.push_back(server_conf);
	}
}


Config::e_error Config::PostValidation() {return Config::ERROR_NONE;}

ServerConfig fill_server(const Directive &server) {(void)server; return ServerConfig();}

