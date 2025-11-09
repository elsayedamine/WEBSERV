#include "Configuration.hpp"

ConfigBlock	validate_cgi_path(const Directive &cgi) { (void)cgi; return ConfigBlock(); }

ConfigBlock	Config::validate_listen(const Directive &listen)
{
	ConfigBlock server;

	if (listen.getValues().size() != 1 || !listen.getChildren().empty())
		return server.valid = false, server;

	std::string host, str_port;
	std::string value = listen.getValues().front();
	size_t pos = value.find(':');
	if (pos != std::string::npos)
		{ host = value.substr(0, pos); str_port = value.substr(pos + 1); }
	else
		str_port = value;
	for (std::size_t i = 0; i < str_port.size(); ++i)
		if (!std::isdigit(str_port[i]))
			return server.valid = false, server;
	int port = std::atoi(str_port.c_str());
	if (port < 1 || port > 65535)
		return server.valid = false, server;
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
	if (!host.empty() && host != "*" && (count != 4 || invalid))
		return server.valid = false, server;
	server.port = port;
	server.host = host;
	return server.valid = true, server;
}

ConfigBlock	Config::validate_clients(const Directive &clients)
{
	ConfigBlock server;

	if (clients.getValues().size() != 1 || !clients.getChildren().empty() || clients.getValues().front().empty())
		return server.valid = false, server;
	std::string value = clients.getValues().front();
	char unit = 0;
	if (std::isalpha(value[value.size() - 1]))
	{
		unit = std::toupper(value[value.size() - 1]);
		value.resize(value.size() - 1);
	}
	for (std::size_t i = 0; i < value.size(); ++i)
		if (!std::isdigit(value[i]))
			return server.valid = false, server;
	long long size = std::atoll(value.c_str());
	if (size <= 0)
		return server.valid = false, server;
	if (unit == 'K')
		size *= 1024;
	else if (unit == 'M')
		size *= 1024 * 1024;
	else if (unit)
		return server.valid = false, server;
	server.client_max_body_size = size;
	server.valid = true;
	return server;
}

ConfigBlock	Config::validate_autoindex(const Directive &autoindex)
{
	ConfigBlock server;

	if (autoindex.getValues().size() != 1 || !autoindex.getChildren().empty())
		return server.valid = false, server;
	std::string value = autoindex.getValues().front();
	if (value == "on" || value == "off")
		server.autoindex = (value == "on");
	else
		return server.valid = false, server;
	server.valid = true;
	return server;
}

ConfigBlock	Config::validate_allowed_methods(const Directive &methods)
{
	ConfigBlock server;

	if (methods.getValues().empty() || !methods.getChildren().empty())
		return server.valid = false, server;
	for (std::size_t i = 0; i < methods.getValues().size(); ++i)
	{
		std::string method = methods.getValues()[i];
		if (method != "GET" && method != "POST" && method != "DELETE")
			return server.valid = false, server;
		server.methods.push_back(method);
	}
	server.valid = true;
	return server;
}

ConfigBlock	Config::validate_upload_enable(const Directive &up)
{
	ConfigBlock server;

	if (up.getValues().size() != 1 || !up.getChildren().empty())
		return server.valid = false, server;
	std::string val = up.getValues().front();
	if (val == "on" || val == "off")
		server.upload_enable = (val == "on");
	else
		return server.valid = false, server;
	server.valid = true;
	return server;
}

ConfigBlock	Config::validate_server_name(const Directive &server_name)
{
	ConfigBlock server;

	server.valid = true;
	if (server_name.getValues().empty() || !server_name.getChildren().empty())
		return server.valid = false, server;
	for (std::size_t i = 0; i < server_name.getValues().size(); ++i)
	{
		std::string name = server_name.getValues()[i];
		if (name.empty())
			{ server.valid = false; break; }
		std::stringstream ss(name);
		std::string segment;
		while (std::getline(ss, segment, '.'))
		{
			if (segment.empty() || segment.size() > 63 || segment[0] == '-' || \
				segment[segment.size() - 1] == '-')
				{ server.valid = false; break; }
			for (std::size_t i = 0; i < segment.size(); ++i)
				if (!std::isalnum(segment[i]) && segment[i] != '-')
					{ server.valid = false; break; }
		}
		if (server.valid == false)
			break ;
		server.server_name.push_back(name);
	}
	return server;
}

ConfigBlock	Config::validate_root(const Directive &root)
{
	ConfigBlock server;

	if (root.getValues().size() != 1 || !root.getChildren().empty())
		return server.valid = false, server;
	std::string path = root.getValues().front();
	if (path.empty() || path[0] != '/')
		return server.valid = false, server;
	server.root = path;
	server.valid = true;
	return server;
}

ConfigBlock	Config::validate_index(const Directive &index)
{
	ConfigBlock server;

	if (index.getValues().empty() || !index.getChildren().empty())
		return server.valid = false, server;
	for (std::size_t i = 0; i < index.getValues().size(); ++i)
	{
		std::string filename = index.getValues()[i];
		if (filename.empty())
			return server.valid = false, server;
		server.index.push_back(filename);
	}
	server.valid = true;
	return server;
}

ConfigBlock	Config::validate_upload_store(const Directive &upload_store)
{
	ConfigBlock server;

	if (upload_store.getValues().size() != 1 || !upload_store.getChildren().empty())
		return server.valid = false, server;
	std::string path = upload_store.getValues().front();
	if (path.empty() || path[0] != '/')
		return server.valid = false, server;
	server.upload_path = path;
	server.valid = true;
	return server;
}

ConfigBlock	Config::validate_return(const Directive &ret)
{
	ConfigBlock server;

	if (ret.getValues().size() != 2 || !ret.getChildren().empty())
		return server.valid = false, server;

	std::string code_str = ret.getValues()[0];
	std::string url = ret.getValues()[1];

	for (std::size_t i = 0; i < code_str.size(); ++i)
		if (!std::isdigit(code_str[i]))
			return server.valid = false, server;
	int code = std::atoi(code_str.c_str());
	if (code < 300 || code > 399 || url.empty())
		return server.valid = false, server;

	server.redirect.first = code;
	server.redirect.second = url;
	server.valid = true;
	return server;
}

ConfigBlock	Config::validate_error_page(const Directive &error_page)
{
    ConfigBlock server;

    const std::vector<std::string> &vals = error_page.getValues();
    if (vals.size() < 2 || !error_page.getChildren().empty() || vals.back().empty())
        return server.valid = false, server;

    for (std::size_t i = 0; i < vals.size() - 1; ++i)
    {
        const std::string &code_str = vals[i];
        for (std::size_t j = 0; j < code_str.size(); ++j)
            if (!std::isdigit(code_str[j]))
                return server.valid = false, server;
        int code = std::atoi(code_str.c_str());
        if (code < 300 || code > 599)
            return server.valid = false, server;
        server.error_page[code] = vals.back();
    }
    server.valid = true;
    return server;
}

ConfigBlock	Config::validate_location(const Directive &location)
{
	ConfigBlock loc;
	std::map<std::string, int> directive_counts;

	if (location.getValues().size() != 1)
		return loc.valid = false, loc;

	loc.prefix = location.getValues().front();

	for (std::size_t i = 0; i < location.getChildren().size(); ++i)
	{
		const Directive &d = location.getChildren()[i];
		const std::string &name = d.getName();
		std::map<std::string, Validators>::iterator it = location_keys.find(d.getName());

		if (it == location_keys.end())
			return loc.valid = false, loc;
		int &count = directive_counts[name];
		if (name != "location" && name != "error_page" && count > 0)
			{ loc.valid = false; return loc; }

		count++;
		ConfigBlock tmp = (this->*(it->second))(d);
		if (!tmp.valid)
			return loc.valid = false, loc;

		if (!tmp.root.empty()) loc.root = tmp.root;
		if (!tmp.client_max_body_size.empty()) loc.client_max_body_size = tmp.client_max_body_size;
		if (!tmp.upload_path.empty()) loc.upload_path = tmp.upload_path;
		if (!tmp.methods.empty()) loc.methods = tmp.methods;
		if (!tmp.redirect.second.empty()) loc.redirect = tmp.redirect;
		if (!tmp.index.empty()) loc.index = tmp.index;
		if (!tmp.autoindex != false && !tmp.autoindex != true) loc.autoindex = tmp.autoindex;
		if (!tmp.error_page.empty()) loc.error_page = tmp.error_page;
	}

	loc.valid = true;
	return loc;
}
