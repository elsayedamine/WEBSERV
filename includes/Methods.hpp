#ifndef METHODS_HPP
#define METHODS_HPP

#include <string>
#include <Response.hpp>
#include <Request.hpp>
#include <ConfigBlock.hpp>
#include <CGI.hpp>

using namespace std;

const string getMimeType(const string &file);
std::string getCodeMessage(int code);
string autoIndex(const string &path, const string &prefix);
pair<string, int> getResource(const string &path);
pair<string, int> processDir(const string &path, const ConfigBlock &location);
pair<string, int> processPath(const string &path, const ConfigBlock &location);
const ConfigBlock *findLocation(const vector<ConfigBlock> &locations, const string &target);
bool normalizeTarget(string &target);
bool compare(const ConfigBlock &a, const ConfigBlock &b);
Response handleGet(Request &request, const string &path, const ConfigBlock &location);
Response handlePost(Request &request, const string &path, const ConfigBlock &location);

#endif