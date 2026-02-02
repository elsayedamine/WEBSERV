#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstddef>
#include <string>
#include <Directive.hpp>
#include <Request.hpp>

std::string num_to_string(std::size_t num);
size_t stringToInt(std::string str);
std::string	strtrim(const std::string &s);
size_t min(size_t a, size_t b);


#endif