#ifndef METHODS_HPP
#define METHODS_HPP

#include <string>
#include <Response.hpp>
#include <Request.hpp>
#include <ConfigBlock.hpp>
#include <CGI.hpp>


const std::string getMimeType(const std::string &file);
std::string autoIndex(const std::string &path, const std::string &prefix);
std::pair<std::string, int> getResource(const std::string &path);
std::pair<std::string, int> processDir(const std::string &path, const ConfigBlock &location);
std::pair<std::string, int> processPath(const std::string &path, const ConfigBlock &location);
const ConfigBlock *findLocation(const std::vector<ConfigBlock> &locations, const std::string &target);
bool normalizeTarget(std::string &target);
bool compare(const ConfigBlock &a, const ConfigBlock &b);
// Methods' Handlers
Response handleGet(const std::string &path, const ConfigBlock &location);
Response handlePost(const Request &request, const std::string &path, const ConfigBlock &location);
Response handlePut(const Request &req, const std::string &path);
Response handleDelete(const std::string &path);


#endif