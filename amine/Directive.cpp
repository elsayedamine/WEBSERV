#include "main.hpp"

std::vector<std::string>	tokenizer(const std::string &str)
{
	std::vector<std::string> tokens;
	std::string	stop = "{};";

	for (std::size_t i = 0; i < str.size();) {
		std::size_t pos = str.find_first_of(stop, i);
		if (pos == std::string::npos)
			pos = str.size();
		std::string token = strtrim(str.substr(i, pos - i));
		if (!token.empty())
			tokens.push_back(token);
		tokens.push_back(str.substr(pos, 1));
		i = pos + 1;
	}
	return tokens;
}

Directive	Directive::DirectiveBuilder(string_it &it, const string_it &end)
{
	Directive Main;

	while (it != end)
	{
		if (*it == "}")
			return ++it, Main;

		Directive dir(*it++);
		while (it != end && *it != ";" && *it != "{" && *it != "}")
			dir.addValue(*it++);
		if (it != end && *it == ";")
			++it;
		else if (it != end && *it == "{")
		{
			++it;
			Directive child = DirectiveBuilder(it, end);
			dir.addChild(child);
		}
		Main.addChild(dir);
		// bayna 3la wed the first loop
		// if (Main.getName().empty())
			// Main = dir;
		// else
			// Main.addChild(dir);
	}
	return Main;
}

Directive::Directive(const char *conf)
{
	std::ifstream file(conf);
	if (!file.is_open()) {
		std::cerr << "Failed to open config file: " << conf << std::endl; return;}

	std::stringstream buffer;
	std::string	content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	std::vector<std::string> tokens = tokenizer(content);
	string_it it = tokens.begin();
	*this = DirectiveBuilder(it, tokens.end());
	this->name = "Main";
}
