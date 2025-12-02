#include <main.hpp>
#include <algorithm>
#include <Methods.hpp>
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
	vector<ConfigBlock> locations = server.locations;
	pair<string, int> body;
	Response *response = NULL;

	stable_sort(locations.begin(), locations.end(), compare);
	{ // Find location and process path
		const ConfigBlock *location;
		string target = request.getTarget();
		string path;
		
		normalizeTarget(target);
		location = findLocation(locations, target);
		if (!location)
			return (new Response(404));
		path = location->root + target.substr(location->prefix.size());
	}
	return (response);
}
