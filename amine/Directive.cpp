#include "Directive.hpp"

std::string AddSpacesAroundDelimiters(const std::string &str)
{
	std::string result;
	std::string delimiters = "{};";

	for (std::size_t i = 0; i < str.size(); ++i) {
		char c = str[i];
		if (delimiters.find(c) != std::string::npos)
		{
			result += ' ';
			result += c;
			result += ' ';
		}
		else
			result += c;
	}
	return result;
}

std::vector<std::string> tokenizer(const std::string &str)
{
	std::string spaced_str = AddSpacesAroundDelimiters(str);
	std::stringstream ss(spaced_str);
	std::vector<std::string> tokens;
	std::string token;

	while (ss >> token)
		tokens.push_back(token);

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
			Directive dumbBlock = DirectiveBuilder(it, end);
			const std::vector<Directive>& realChildren = dumbBlock.getChildren();
			for (std::size_t i = 0; i < realChildren.size(); ++i)
				dir.addChild(realChildren[i]);
		}
		Main.addChild(dir);
	}
	return Main;
}

void	ValidateTokens(const std::vector<std::string> & tokens)
{
	if (tokens.empty())
		throw ConfigException("Config file is empty.");

	std::vector<std::size_t> brace_stack;
	
	NextState state = EXPECT_DIRECTIVE;
	for (std::size_t i = 0; i < tokens.size(); ++i)
	{
		if (tokens[i] == "{")
		{
			if (state == EXPECT_DIRECTIVE)
				throw ConfigException("Unexpected '{' .");
			brace_stack.push_back(i);
			state = EXPECT_DIRECTIVE;
		}
		else if (tokens[i] == "}")
		{
			if (state == EXPECT_TERMINATOR_OR_VALUE)
				throw ConfigException("Missing ';' before '}' .");
			if (brace_stack.empty())
				throw ConfigException("Unexpected '}' .");
			// if (i > 0 && tokens[i - 1] == "{")
			// 	throw ConfigException("Empty block '{}' is not allowed .");
			state = EXPECT_DIRECTIVE;
			brace_stack.pop_back();
		}
		else if (tokens[i] == ";")
		{
			if (state == EXPECT_DIRECTIVE)
				throw ConfigException("Unexpected ';' .");
			state = EXPECT_DIRECTIVE;
		}
		else
			state = EXPECT_TERMINATOR_OR_VALUE;
	}
	if (state == EXPECT_TERMINATOR_OR_VALUE)
		throw ConfigException("Unexpected end of file (expected ';' or '{').");
	if (!brace_stack.empty())
		throw ConfigException("Mismatched '{' (from token " + num_to_string(brace_stack.front()) + ") at end of file.");
}

Directive::Directive(const char *conf)
{
	std::ifstream file(conf);
	if (!file.is_open()) {
		std::cerr << "Failed to open config file: " << conf << std::endl; return;}

	std::stringstream buffer;
	std::string	content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	std::vector<std::string> tokens = tokenizer(content);

	ValidateTokens(tokens);

	string_it it = tokens.begin();
	*this = DirectiveBuilder(it, tokens.end());
	this->name = "Main";
}
