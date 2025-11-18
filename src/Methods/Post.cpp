#include <main.hpp>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

string getPath(string dir, const vector<ConfigBlock> locations) {
	static map<string, int> entries;
	struct stat st;

	for (vector<ConfigBlock>::iterator it; it != locations.end(); ++it) {
		string path = it->prefix + dir;
		if (!stat(path.c_str(), &st) && S_ISDIR(st.st_mode)) {
			if (entries.find(dir) == entries.end())
				entries[dir] = 0;
			entries[dir]++;
			return (path + num_to_string(entries[dir]));
		}
	}
	return ("");
}

Response *handlePost(Request &request, const ConfigBlock &server) {
	string path;
	Response *response;

	path = getPath(request.getTarget(), server.locations);
	if (path.empty()) {
		if (errno == EACCES)
			return (new Response(403));
		return (new Response(404));
	}
	{
		int fd = open(path.c_str(), O_WRONLY);
		write(fd, request.getBody().c_str(), request.getBody().length());
		response = new Response(201);
		response->setBody(path);
		response->setHeader("Content-Type", "text/plain");
		response->setHeader("Content-Length", num_to_string(response->getBody().size()));
	}
	return (response);
}
