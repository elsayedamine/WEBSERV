#ifndef MAIN_HPP
#define MAIN_HPP

#include "Request.hpp"

// Function prototypes
Request::Method stringToMethod(const string& methodStr);
vector<string> tokenize(stringstream stream);
void handleConnection(int fd);

#endif // MAIN_HPP