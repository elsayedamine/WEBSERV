#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <Directive.hpp>
#include <Request.hpp>

std::string num_to_string(std::size_t num);
int stringToInt(std::string str);
std::string	strtrim(const std::string &s);

// printing
std::ostream &operator<<(std::ostream &os, const Request &req);

#endif