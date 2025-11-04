#ifndef MAIN_HPP
#define MAIN_HPP

#include "Request.hpp"

// Function prototypes
void parseRequest(int fd);
Request::Method stringToMethod(const string& methodStr);
vector<string> tokenize(const string& line);
string readline(int fd, bool includeDelimiter);
Request *parseRequestLine(string line);

#endif // MAIN_HPP