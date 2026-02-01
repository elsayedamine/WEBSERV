#include <Directive.hpp>

std::vector<std::string> tokenizer(const std::string &content)
{
	bool in_single = false;
	bool in_double = false;
	std::string current;
	std::vector<std::string> tokens;

	for (std::size_t i = 0; i < content.size(); ++i)
	{
		char c = content[i];
		if (c == '\'' && !in_double)
			{ in_single = !in_single; current += c; continue; }
		if (c == '"' && !in_single)
			{ in_double = !in_double; current += c; continue; }
		if (c == '\n' && (in_single || in_double))
			throw std::runtime_error("Newline inside unclosed quote.");
		if (!in_single && !in_double)
		{
			if (std::isspace(static_cast<unsigned char>(c))) {
				if (!current.empty())
					{ tokens.push_back(current); current.clear(); }
			}
			else if (c == '{' || c == '}' || c == ';') {
				if (!current.empty()) {
					tokens.push_back(current); current.clear(); }
				tokens.push_back(std::string(1, c));
			}
			else
				current += c;
		}
		else
			current += c;
	}
	if (!current.empty())
		tokens.push_back(current);
	if (in_single)
		throw std::runtime_error("Unclosed single quote in config file.");
	if (in_double)
		throw std::runtime_error("Unclosed double quote in config file.");
	return tokens;
}

void	strip_comments(std::string& content)
{
	bool	in_single = false;
	bool	in_double = false;
	std::string out;

	for (size_t i = 0; i < content.size(); i++)
	{
		char c = content[i];
		if (c == '\'' && !in_double) in_single = !in_single;
		else if (c == '"' && !in_single) in_double = !in_double;
		else if (c == '#' && !in_single && !in_double)
		{
			while (i < content.size() && content[i] != '\n') i++;
			out += '\n';
			continue;
		}
		out += c;
	}
	content = out;
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

void	ValidateTokens(const std::vector<std::string> &tokens)
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
	if (!file.is_open())
	{
		std::string fil(conf);
		std::string error = str("Failed to open config file: ") + fil;
		throw std::runtime_error(error);
	}

	std::stringstream buffer;
	std::vector<std::string> tokens;
	string_it it;

	std::string	content((bufIt(file)), bufIt());
	strip_comments(content);
	tokens = tokenizer(content);
	ValidateTokens(tokens);

	it = tokens.begin();
	*this = DirectiveBuilder(it, tokens.end());
	this->name = "Main";
}
