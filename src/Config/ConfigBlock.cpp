#include <Configuration.hpp>

ConfigBlock	validate_listen(const Directive &listen)
{
	ConfigBlock server;

	if (listen.getValues().size() != 1 || !listen.getChildren().empty())
		return server.err = ERROR_INVALID_HOST, server;

	std::string host, str_port;
	std::string value = listen.getValues().front();
	size_t pos = value.find(':');

	if (pos != std::string::npos)
		{ host = value.substr(0, pos); str_port = value.substr(pos + 1); }
	else
		{ str_port = value; host = "0.0.0.0"; }
	
	if (host == "localhost") host = "127.0.0.1";
	if (host.empty() || host == "*") host = "0.0.0.0";
	if (pos == std::string::npos && str_port.find('.') != std::string::npos)
		{ host = str_port; str_port = "80"; }
	if (str_port.empty())
		return server.err = ERROR_INVALID_PORT, server;

	for (std::size_t i = 0; i < str_port.size(); ++i)
		if (!std::isdigit(str_port[i]))
			return server.err = ERROR_INVALID_PORT, server;
	int port = std::atoi(str_port.c_str());
	if (port < 1 || port > 65535)
		return server.err = ERROR_INVALID_PORT, server;
	std::stringstream ss(host);
	std::string segment;
	int count = 0, invalid = false;
	while (!host.empty() && std::getline(ss, segment, '.'))
	{
		if (++count > 4 || segment.empty() || segment.size() > 3)
			{ invalid = true; break; }
		for (std::size_t i = 0; i < segment.size(); ++i)
			if (!std::isdigit(segment[i]))
				{ invalid = true; break; }
		if (invalid)
			break;
		int n = std::atoi(segment.c_str());
		if (n < 0 || n > 255)
			{ invalid = true; break; }
	}
	if (!host.empty() && (count != 4 || invalid))
		return server.err = ERROR_INVALID_HOST, server;
	server.port = port;
	server.host = host;
	server.err = ERROR_NONE;
	return server;
}

ConfigBlock	validate_clients(const Directive &clients)
{
	ConfigBlock server;

	if (clients.getValues().size() != 1 || !clients.getChildren().empty() || clients.getValues().front().empty())
		return server.err = ERROR_INVALID_CLIENT_MAX_BODY_SIZE, server;
	std::string value = clients.getValues().front();
	char unit = 0;
	if (std::isalpha(value[value.size() - 1]))
	{
		unit = std::toupper(value[value.size() - 1]);
		value.resize(value.size() - 1);
	}
	for (std::size_t i = 0; i < value.size(); ++i)
		if (!std::isdigit(value[i]))
			return server.err = ERROR_INVALID_CLIENT_MAX_BODY_SIZE, server;
	long long size = std::atoll(value.c_str());
	if (size <= 0)
		return server.err = ERROR_INVALID_CLIENT_MAX_BODY_SIZE, server;
	if (unit == 'K' || unit == 'k')
		size *= 1000;
	else if (unit == 'M' || unit == 'm')
		size *= 1000 * 1000;
	else if (unit)
		return server.err = ERROR_INVALID_CLIENT_MAX_BODY_SIZE, server;
	server.client_max_body_size = size;
	server.err = ERROR_NONE;
	return server;
}

ConfigBlock	validate_autoindex(const Directive &autoindex)
{
	ConfigBlock server;

	if (autoindex.getValues().size() != 1 || !autoindex.getChildren().empty())
		return server.err = ERROR_INVALID_AUTOINDEX, server;
	std::string value = autoindex.getValues().front();
	if (value == "on" || value == "off")
		server.autoindex = (value == "on");
	else
		return server.err = ERROR_INVALID_AUTOINDEX, server;
	server.err = ERROR_NONE;
	return server;
}

ConfigBlock	validate_allowed_methods(const Directive &methods)
{
	ConfigBlock server;

	if (methods.getValues().empty() || !methods.getChildren().empty())
		return server.err = ERROR_INVALID_ALLOWED_METHODS, server;
	for (std::size_t i = 0; i < methods.getValues().size(); ++i)
	{
		std::string method = methods.getValues()[i];
		if (method != "GET" && method != "POST" && method != "DELETE")
			return server.err = ERROR_INVALID_ALLOWED_METHODS, server;
		server.methods.push_back(method);
	}
	server.err = ERROR_NONE;
	return server;
}

ConfigBlock	validate_upload_enable(const Directive &up)
{
	ConfigBlock server;

	if (up.getValues().size() != 1 || !up.getChildren().empty())
		return server.err = ERROR_INVALID_UPLOAD_STATE, server;
	std::string val = up.getValues().front();
	if (val == "on" || val == "off")
		server.upload_enable = (val == "on");
	else
		return server.err = ERROR_INVALID_UPLOAD_STATE, server;
	server.err = ERROR_NONE;
	return server;
}

ConfigBlock validate_server_name(const Directive &server_name)
{
	ConfigBlock server;
	server.err = ERROR_NONE;
	const std::vector<std::string>& values = server_name.getValues();

	if (values.empty() || !server_name.getChildren().empty())
		return server.err = ERROR_INVALID_SERVER_NAME, server;

	for (std::size_t i = 0; i < values.size(); ++i)
	{
		std::string name = values[i];
		if (name.empty() || name.size() > 253)
			return server.err = ERROR_INVALID_SERVER_NAME, server;

		server.server_name.push_back(name);
	}
	return server;
}

ConfigBlock	validate_root(const Directive &root)
{
	ConfigBlock server;

	if (root.getValues().size() != 1 || !root.getChildren().empty())
		return server.err = ERROR_INVALID_ROOT, server;
	std::string path = root.getValues().front();
	if (path.empty() || path[0] != '/')
		return server.err = ERROR_INVALID_ROOT, server;
	server.root = path;
	server.err = ERROR_NONE;
	return server;
}

ConfigBlock	validate_index(const Directive &index)
{
	ConfigBlock server;

	if (index.getValues().empty() || !index.getChildren().empty())
		return server.err = ERROR_INVALID_INDEX, server;
	for (std::size_t i = 0; i < index.getValues().size(); ++i)
	{
		std::string filename = index.getValues()[i];
		if (filename.empty())
			return server.err = ERROR_INVALID_INDEX, server;
		server.index.push_back(filename);
	}
	server.err = ERROR_NONE;
	return server;
}

ConfigBlock	validate_upload_store(const Directive &upload_store)
{
	ConfigBlock server;

	if (upload_store.getValues().size() != 1 || !upload_store.getChildren().empty())
		return server.err = ERROR_INVALID_UPLOAD_PATH, server;
	std::string path = upload_store.getValues().front();
	if (path.empty() || path[0] != '/')
		return server.err = ERROR_INVALID_UPLOAD_PATH, server;
	server.upload_path = path;
	server.err = ERROR_NONE;
	return server;
}

ConfigBlock validate_return(const Directive &ret)
{
	ConfigBlock server;
	const std::vector<std::string>& values = ret.getValues();
	size_t size = values.size();

	if (size < 1 || size > 2 || !ret.getChildren().empty())
		return server.err = ERROR_INVALID_RETURN, server;

	std::string val1 = values[0];
	
	bool is_numeric = !val1.empty();
	for (size_t i = 0; i < val1.size(); ++i) {
		if (!std::isdigit(val1[i])) { is_numeric = false; break; } }

	if (is_numeric)
	{
		int code = std::atoi(val1.c_str());
		if (code < 100 || code > 599)
			return server.err = ERROR_INVALID_RETURN, server;

		server.ret.first = code;

		server.ret.second = "";
		if (size == 2)
			server.ret.second = values[1];
	}
	else
	{
		if (size > 1) 
			return server.err = ERROR_INVALID_RETURN, server;

		server.ret.first = 302;
		server.ret.second = val1;
	}

	server.err = ERROR_NONE;
	return server;
}

ConfigBlock	validate_error_page(const Directive &error_page)
{
	ConfigBlock server;

	const std::vector<std::string> &vals = error_page.getValues();
	if (vals.size() < 2 || !error_page.getChildren().empty() || vals.back().empty())
		return server.err = ERROR_INVALID_ERROR_PAGE, server;

	for (std::size_t i = 0; i < vals.size() - 1; ++i)
	{
		const std::string &code_str = vals[i];
		for (std::size_t j = 0; j < code_str.size(); ++j)
			if (!std::isdigit(code_str[j]))
				return server.err = ERROR_INVALID_ERROR_PAGE, server;
		int code = std::atoi(code_str.c_str());
		if (code < 300 || code > 599)
			return server.err = ERROR_INVALID_ERROR_PAGE, server;
		server.error_page[code] = vals.back();
	}
	server.err = ERROR_NONE;
	return server;
}

ConfigBlock	validate_cgi(const Directive &cgi)
{
	ConfigBlock server;

	if (!cgi.getChildren().empty() || cgi.getValues().size() != 2)
		return server.err = ERROR_INVALID_CGI, server;

	const std::string &extension = cgi.getValues()[0];
	const std::string &interpreter = cgi.getValues()[1];

	if (extension[0] != '.' || interpreter.empty())
		return server.err = ERROR_INVALID_CGI, server;

	struct stat s;
	if (stat(interpreter.c_str(), &s) != 0 || !S_ISREG(s.st_mode) || access(interpreter.c_str(), X_OK) != 0)
		return server.err = ERROR_INVALID_CGI, server;
	server.cgi[extension] = interpreter;
	server.err = ERROR_NONE;
	return server;
}

ConfigBlock	validate_location(const Directive &location)
{
	ConfigBlock LocationContainer;
	ConfigBlock loc;
	std::map<std::string, Validators> location_keys;
	std::map<std::string, int> directive_counts;

	if (location.getValues().size() != 1) {
		LocationContainer.err = ERROR_INVALID_LOCATION;
		return LocationContainer;
	}
	loc.prefix = location.getValues().front();
	fill_location_keys(location_keys);
	for (std::size_t i = 0; i < location.getChildren().size(); ++i)
	{
		const Directive &d = location.getChildren()[i];
		const std::string &name = d.getName();
		std::map<std::string, Validators>::iterator it = location_keys.find(d.getName());

		if (it == location_keys.end())
			return LocationContainer.err = ERROR_INVALID_KEY_IN_LOCATION, LocationContainer;
		int &count = directive_counts[name];
		if (name != "location" && name != "error_page" && count > 0)
			{ LocationContainer.err = ERROR_DUPLICATE_KEY; return LocationContainer; }

		count++;
		ConfigBlock tmp = it->second(d);
		if (tmp.err) { LocationContainer.err = tmp.err; return LocationContainer ;}
		if (!tmp.root.empty()) loc.root = tmp.root;
		if (tmp.client_max_body_size != -1) loc.client_max_body_size = tmp.client_max_body_size;
		if (!tmp.upload_path.empty()) loc.upload_path = tmp.upload_path;
		if (!tmp.methods.empty()) loc.methods = tmp.methods;
		if (!tmp.ret.second.empty()) loc.ret = tmp.ret;
		if (!tmp.index.empty()) loc.index = tmp.index;
		if (tmp.autoindex != -1) loc.autoindex = tmp.autoindex;
		if (tmp.upload_enable != -1) loc.upload_enable = tmp.upload_enable;
		if (!tmp.error_page.empty()) loc.error_page = tmp.error_page;
	}
	loc.err = ERROR_NONE;
	LocationContainer.locations.push_back(loc);
	LocationContainer.err = ERROR_NONE;
	return LocationContainer;
}

ConfigBlock::ConfigBlock(const Directive &server) : err((e_error)0), port(0), autoindex(-1), upload_enable(-1)
{
	std::map<std::string, int> directive_counts;
	std::map<std::string, Validators> server_keys;
	
	fill_server_keys(server_keys);
	for (std::size_t i = 0; i < server.getChildren().size(); ++i) {
		const Directive &d = server.getChildren()[i];
		const std::string &name = d.getName();
		const std::map<std::string, Validators>::iterator & it = server_keys.find(name);
		if (it == server_keys.end()) {
			err = ERROR_UNKNOWN_KEY;
			return ;
		}
		int &count = directive_counts[name];
		if (name != "location" && name != "error_page" && count > 0) {
			err = ERROR_DUPLICATE_KEY;
			return ;
		}
		count++;
		ConfigBlock tmp = it->second(d);
		if (tmp.err) { this->err = tmp.err; return ; }
		if (!tmp.error_page.empty()) this->error_page = tmp.error_page;
		if (tmp.port) this->port = tmp.port;
		if (!tmp.host.empty()) this->host = tmp.host;
		if (!tmp.root.empty()) this->root = tmp.root;
		if (tmp.client_max_body_size != -1) this->client_max_body_size = tmp.client_max_body_size;
		if (!tmp.upload_path.empty()) this->upload_path = tmp.upload_path;
		if (!tmp.methods.empty()) this->methods = tmp.methods;
		if (!tmp.ret.second.empty()) this->ret = tmp.ret;
		if (!tmp.server_name.empty()) this->server_name = tmp.server_name;
		if (!tmp.index.empty()) this->index = tmp.index;
		if (tmp.autoindex != -1) this->autoindex = tmp.autoindex; 
		if (tmp.upload_enable != -1) this->upload_enable = tmp.upload_enable; 
		if (!tmp.locations.empty()) this->locations.insert(this->locations.end(), tmp.locations.begin(), tmp.locations.end());
		// check (overload the operator= for these ifs)
	}
}
