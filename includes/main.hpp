#ifndef MAIN_HPP
#define MAIN_HPP

#include <sstream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <Request.hpp>
#include <Response.hpp>
#include <Server.hpp>

vector<string> tokenize(string& line);
pair<string, string> parseHeader(string& header);
Request parseRequest(string& data);
int validateRequest(Request &request, const ConfigBlock &server);
void handleConnection(int fd, const std::vector<ConfigBlock> &candidates);
std::string	strtrim(const std::string &s);
Response processRequest(Request& request, const ConfigBlock &server);
string getCodeMessage(int code);

#endif