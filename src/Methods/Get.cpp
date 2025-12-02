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
		resource.append(buf);
	return (make_pair(resource, 0));
}

pair<string, int> processDir(const string &path, const ConfigBlock &location) {
	if (!location.index.empty()) {
		vector<string>::const_iterator it = location.index.begin();
	
		while (it != location.index.end()) {
			string index = path + '/' + *it;
			cout << index << endl;
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

Response handleGet(Request &request, const ConfigBlock &server) {
	vector<ConfigBlock> locations = server.locations;
	pair<string, int> body;

	stable_sort(locations.begin(), locations.end(), compare);
	{ // Find location and process path
		const ConfigBlock *location;
		string target = request.getTarget();
		string path;
		
		normalizeTarget(target);
		location = findLocation(locations, target);
		if (!location)
			return (Response(404));
		path = location->root + target.substr(location->prefix.size());
		if (path[path.size() - 1] == '/')
			body = processDir(path, *location);
		else
			body = processPath(path, *location);
	}
	{ // Form response
		Response response(200);
		
		if (body.first.empty())
			return (Response(402 + body.second));
		response.setBody(body.first);
		response.setHeader("Content-Length", num_to_string(body.first.size()));
		response.setHeader("Content-Type", getMimeType(request.getTarget()));
		return (response);
	}
}
