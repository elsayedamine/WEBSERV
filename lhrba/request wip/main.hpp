#ifndef MAIN_HPP
#define MAIN_HPP

#include "Request.hpp"

// Function prototypes
void parseRequest(int fd);
Request::Method stringToMethod(const string& methodStr);
Request parseRequestLine(string line);

#endif // MAIN_HPP