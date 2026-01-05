#include <fcntl.h>
#include <sys/stat.h>
#include <main.hpp>
#include <dirent.h>
#include <algorithm>
#include <Methods.hpp>

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
		if (name == "." || name == "..")
			continue;
		body += "<li>" + name + "</li>\n";
	}
	body += "</ul>\n</body>\n</html>";
	return (body);
}

pair<string, int> getResource(const string path) {
	int fd;
	char buf[1024];
	ssize_t readSize;
	string resource;

	fd = open(path.c_str(), O_RDONLY);
	if (fd == -1)
		return (make_pair("", (errno == EACCES) + 2 * (errno == ENOENT)));
	while ((readSize = read(fd, buf, 1024)) > 0)
		resource.append(buf, static_cast<size_t>(readSize));
	return (make_pair(resource, 0));
}

pair<string, int> processDir(const string &path, const ConfigBlock &location) {
	if (!location.index.empty()) {
		vector<string>::const_iterator it = location.index.begin();
	
		while (it != location.index.end()) {
			string index = path + '/' + *it;
			if (!access(index.c_str(), F_OK))
				return (getResource(index));
			++it;
		}
	}
	if (location.autoindex)
		return (make_pair(autoIndex(path), 0));
	return (make_pair("", 1));
}

pair<string, int> processPath(const string &path, const ConfigBlock &location) {
	struct stat st;

	if (stat(path.c_str(), &st) == -1)
		return (make_pair("", (errno == EACCES) + 2 * (errno == ENOENT)));
	if (S_ISDIR(st.st_mode))
		return (processDir(path, location));
	return (getResource(path));
}

string setType(const string &path, const ConfigBlock &location) {
	struct stat st;

	stat(path.c_str(), &st);
	if (S_ISDIR(st.st_mode)) {
		vector<string>::const_iterator it = location.index.begin();

		while (!location.index.empty() && it != location.index.end()) {
			string index = path + '/' + *it;
			if (!access(index.c_str(), F_OK))
				return (getMimeType(index));
			++it;
		}
		return ("text/html");
	} else
		return (getMimeType(path));
	return("application/octet-stream");
}

Response handleGet(Request &request, const string &path, const ConfigBlock &location) {
	pair<string, int> body;
	
	(void)request;
	{ // Find location and process path
		if (path[path.size() - 1] == '/')
			body = processDir(path, location);
		else
			body = processPath(path, location);
	}
	{ // Form response
		Response response(200);
		
		if (body.first.empty())
			return (Response(402 + body.second));
		response.setBody(body.first);
		response.setHeader("Content-Length", num_to_string(body.first.size()));
		response.setHeader("Content-Type", setType(path, location));
		return (response);
	}
}
