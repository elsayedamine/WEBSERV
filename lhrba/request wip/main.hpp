#ifndef MAIN_HPP
#define MAIN_HPP

#include <sstream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include "Request.hpp"
#include "Response.hpp"

// Function prototypes
Request::Method stringToMethod(const string& methodStr);
vector<string> tokenize(string& line);
pair<string, string> parseHeader(string& header);
Request* parseRequest(string& data);
void handleConnection(int fd);

Response *processRequest(Request& request);

#endif // MAIN_HPP