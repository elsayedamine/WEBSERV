#include <fcntl.h>
#include <sys/stat.h>
#include <main.hpp>
#include <dirent.h>
#include <algorithm>

const string getMimeType(const string &file) {
	static map<string, string> types;
	if (types.empty()) {
		types["html"] =  "text/html";
		types["htm"] =   "text/html";
		types["css"] =   "text/css";
		types["js"] =    "application/javascript";
		types["mjs"] =   "application/javascript";
		types["json"] =  "application/json";
		types["txt"] =   "text/plain";
		types["xml"] =   "application/xml";
		types["jpg"] =   "image/jpeg";
		types["jpeg"] =  "image/jpeg";
		types["png"] =   "image/png";
		types["gif"] =   "image/gif";
		types["svg"] =   "image/svg+xml";
		types["ico"] =   "image/vnd.microsoft.icon";
		types["bmp"] =   "image/bmp";
		types["webp"] =  "image/webp";
		types["mp4"] =   "video/mp4";
		types["mpeg"] =  "video/mpeg";
		types["mp3"] =   "audio/mpeg";
		types["wav"] =   "audio/wav";
		types["ogg"] =   "audio/ogg";
		types["pdf"] =   "application/pdf";
		types["zip"] =   "application/zip";
		types["tar"] =   "application/x-tar";
		types["gz"] =    "application/gzip";
		types["7z"] =    "application/x-7z-compressed";
		types["csv"] =   "text/csv";
		types["woff"] =  "font/woff";
		types["woff2"] = "font/woff2";
		types["ttf"] =   "font/ttf";
	}
	
	{
		size_t dot = file.find_last_of('.');

		if (dot == file.npos)
			return ("application/octet-stream");
		string ext = file.substr(dot + 1);
		if (types.find(ext) == types.end())
			return ("application/octet-stream");
		return (types[ext]);
	}
}

string autoIndex(string path) {
	DIR *dir = opendir(path.c_str());
	struct dirent *ent;
	string body = 
		"<!doctype html>\n"
		"<html>\n<head>\n<meta charset=\"utf-8\">\n"
		"<title>Index of " + path + "</title>\n</head>\n<body>\n"
		"<h1>Index of " + path + "</h1>\n"
		"<ul>\n";

	while (1) {
		ent = readdir(dir);
		if (!ent)
			break;
		string name(ent->d_name);
		if (name == ".")
			continue;
		body += "<li>" + name + "</li>\n";
	}
	body += "</ul>\n</body>\n</html>";
}

string processDir(const string &path, const string &target, const ConfigBlock &location) {
	if (location.index.empty())
		
}

const ConfigBlock *findLocation(const vector<ConfigBlock> &locations, const string &target) {
	vector<ConfigBlock>::const_iterator it = locations.begin();

	while (it == locations.end()) {
		if (!target.compare(0, it->prefix.size(), it->prefix))
			break;
		++it;
	}
	if (it == locations.end())
		return (NULL);
	return (&locations[it - locations.begin()]);
}

// Normalize target (remove "." and "..", remove multiple /)
// Match target against locations and choose the longest
// If there's a trailing /, treat as a directory. otherwise, still check if it's either and apply logic accordingly
// Remove location prefix and process the remainder in the root directory of the location

bool normalizeTarget(string &target) {
	if (target[0] != '/')
		return (false);
	while (1) {
		size_t pos = target.find("/./");
		if (pos == string::npos)
			pos = target.find("/../");
		if (pos == string::npos)
			break;
		target.erase(pos, 2 + (target[pos + 2] == '.'));
	}
	while (1) {
		size_t pos = target.find("//");
		if (pos == string::npos)
			break;
		target.erase(pos, 1);
	}
	return (true);
}

static size_t depthPrefix(const string &prefix) {
	size_t count = 0;
	size_t i = 0;
	while (i < prefix.size()) {
		while (i < prefix.size() && prefix[i] == '/')
			++i;
		if (i >= prefix.size())
			break;
		size_t j = prefix.find('/', i);
		if (j == string::npos) {
			++count;
			break;
		}
		++count;
		i = j + 1;
	}
	return (count);
}

bool compare(const ConfigBlock &a, const ConfigBlock &b) {
	size_t da = depthPrefix(a.prefix);
	size_t db = depthPrefix(b.prefix);
	if (da != db)
		return (da > db);
	return (a.prefix.size() > b.prefix.size());
}

Response *handleGet(Request &request, const ConfigBlock &server) {
	vector<ConfigBlock> locations = server.locations;
	string body;

	stable_sort(locations.begin(), locations.end(), compare);
	{
		const ConfigBlock *location;
		string target = request.getTarget();
		string path;
		
		normalizeTarget(target);
		location = findLocation(locations, target);
		if (!location)
			return (new Response(404));
		path = location->root + target.substr(location->prefix.size());
		if (path[path.size() - 1] == '/')
			body = processDir(path, target, *location);
		// else
			// body = 
	}
	{

	}
	return (NULL);
}
