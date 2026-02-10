#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>
#include <Methods.hpp>

std::string autoindexMakeEntry(const std::string &name, const std::string &prefix) {
	std::string entry = "<a class=\"text-3xl my-3 hover:text-blue-400\" href=\"";

	entry += prefix + (prefix[prefix.size() - 1] != '/' ? "/" : "") + name + "\">" + name + "</a>\n";
	return (entry);
}

std::string autoIndex(DIR *dir, const std::string &prefix) {
	struct dirent *ent;
	std::string body = std::string(
		"<!DOCTYPE html>"
		"<html lang=\"en\">"
		"<head>"
		"<meta charset=\"UTF-8\" />"
		"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />"
		"<script src=\"https://cdn.tailwindcss.com\"></script>"
		"<title>autoindex</title>"
		"</head>"
		"<body class=\"bg-gray-200 text-center\">"
		"<div class=\"mx-auto min-h-screen max-w-3xl bg-blue-100 py-10 border-l-4 border-r-4 border-black px-6\">"
		"<h1 class=\"font-bold text-8xl\">Index of</h1>"
		"<h1 class=\"font-bold text-5xl my-8\">") + prefix + std::string(
		"</h1>"
		"<div class=\"flex flex-col bg-white max-w-xl mx-auto border-2 border-black rounded-2xl\">"
	);

	int count = 0;
	while ((ent = readdir(dir)) != NULL) {
		std::string name(ent->d_name);
		if (name == "." || name == "..")
			continue;
		body += autoindexMakeEntry(name, prefix);
		++count;
	}
	if (!count)
		body += "<p class=\"text-3xl my-3\">This directory is empty</p>";
	closedir(dir);
	body += "</div></div></body></html>";
	return body;
}

std::pair<std::string, int> getResource(const std::string &path) {
	int fd;
	char buf[1024];
	ssize_t readSize;
	std::string resource;

	fd = open(path.c_str(), O_RDONLY);
	if (fd == -1)
		return (std::make_pair("", (errno == EACCES) + 2 * (errno == ENOENT)));
	while ((readSize = read(fd, buf, 1024)) > 0)
		resource.append(buf, static_cast<size_t>(readSize));
	return (std::make_pair(resource, 0));
}

std::pair<std::string, int> processDir(const std::string &path, const ConfigBlock &location) {
	if (!location.index.empty()) {
		std::vector<std::string>::const_iterator it = location.index.begin();
	
		while (it != location.index.end()) {
			std::string index = path + '/' + *it;
			if (!access(index.c_str(), F_OK))
				return (getResource(index));
			++it;
		}
	}
	else if (location.autoindex) {
		DIR *dir = opendir(path.c_str());

		if (!dir)
			return (std::make_pair("", 2));
		return (std::make_pair(autoIndex(dir, location.prefix), 0));
	}
	return (std::make_pair("", 1));
}

std::pair<std::string, int> processPath(const std::string &path, const ConfigBlock &location) {
	struct stat st;

	if (stat(path.c_str(), &st) == -1)
		return (std::make_pair("", (errno == EACCES) + 2 * (errno == ENOENT)));
	if (S_ISDIR(st.st_mode))
		return (processDir(path, location));
	return (getResource(path));
}

std::string setType(const std::string &path, const ConfigBlock &location) {
	struct stat st;

	stat(path.c_str(), &st);
	if (S_ISDIR(st.st_mode)) {
		std::vector<std::string>::const_iterator it = location.index.begin();

		while (!location.index.empty() && it != location.index.end()) {
			std::string index = path + '/' + *it;
			if (!access(index.c_str(), F_OK))
				return (getMimeType(index));
			++it;
		}
		return ("text/html");
	} else
		return (getMimeType(path));
	return("application/octet-stream");
}

Response handleGet(const std::string &path, const ConfigBlock &location) {
	std::pair<std::string, int> body;
	
	{ // Find location and process path
		if (path[path.size() - 1] == '/')
			body = processDir(path, location);
		else
			body = processPath(path, location);
	}
	{ // Form response
		Response response(200);
		
		if (body.second)
			return (Response(402 + body.second));
		response.setBody(body.first);
		response.setHeader("Content-Type", setType(path, location));
		return (response);
	}
}
