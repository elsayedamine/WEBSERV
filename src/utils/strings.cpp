#include <Directive.hpp>


std::string	strtrim(const std::string &s)
{
	size_t start = 0;
	while (start < s.size() && std::isspace((unsigned char)(s[start])))
		++start;
	size_t end = s.size();
	while (end > start && std::isspace((unsigned char)(s[end - 1])))
		--end;
	return s.substr(start, end - start);
}

std::string num_to_string(std::size_t num)
{
	std::stringstream ss;
	ss << num;
	return ss.str();
}

int stringToInt(string str) {
	size_t n = 0;
	std::istringstream iss(str);
	iss >> n;
	return (n);
}