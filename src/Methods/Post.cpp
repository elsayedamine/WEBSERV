#include <main.hpp>
#include <algorithm>
#include <Methods.hpp>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

string createResource(const string &path, const string &target, const string &body) {
	static map<string, int> entries;

	if (entries.find(target) == entries.end())
		entries[target] = 0;
	entries[target]++;
	{
		string filename = path + '/' + num_to_string(entries[target]);
		int fd = open(filename.c_str(), O_WRONLY | O_CREAT);

		if (fd == -1)
			return ("");
		write(fd, body.c_str(), body.size());
	}
	return (target + '/' + num_to_string(entries[target]));
}

Response handlePost(Request &request, const string &path, const ConfigBlock &location) {
	(void)location;
	string body;

	body = createResource(path, request.getTarget(), request.getBody());
	{ // Form response
		Response response(201);

		if (body.empty())
			cout << "test\n";
		response.setBody(body);
		response.setHeader("Content-Length", num_to_string(body.size()));
		response.setHeader("Content-Type", getMimeType(request.getTarget()));
		return (response);
	}
}
