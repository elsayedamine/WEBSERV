#include <fcntl.h>
#include <sys/stat.h>
#include <main.hpp>
#include <dirent.h>

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

pair<string, int> findResource(const vector<ConfigBlock> locations, const string &resource) {
	struct stat st;
	pair<string, int> ret;
	
	if (stat(resource.c_str(), &st)) {
		ret.first = resource;
		if (S_ISDIR(st.st_mode)) {
			ret.second = 2;
			return (ret);
		}
		ret.second = 1;
		return (ret);
	}
	for (vector<ConfigBlock>::iterator it; it != locations.end(); ++it) {
		string path = it->prefix + resource;
		if (stat(path.c_str(), &st)) {
			ret.first = path;
			if (S_ISDIR(st.st_mode)) {
				ret.second = 2;
				return (ret);
			}
			ret.second = 1;
			return (ret);
		}
	}
	return (make_pair("", -1));
}

// Normalize target (remove "." and "..", remove multiple /)
// Match target against locations and choose the longest
// If there's a trailing /, treat as a directory. otherwise, still check if it's either and apply logic accordingly
// Remove location prefix and process the remainder in the root directory of the location

Response *handleGet(Request &request, const ConfigBlock &server) {
	pair<string, int> resource;

	resource = findResource(server.locations, request.getTarget());
	{
		char buffer[1024];
		ssize_t readSize;
		string body;
		Response *response;

		response = new Response(200);
		response->setBody(body);
		response->setHeader("Content-Type", getMimeType(request.getTarget()));
		response->setHeader("Content-Length", num_to_string(response->getBody().size()));
		return (response);
	}
}
