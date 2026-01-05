#include <main.hpp>
#include <Methods.hpp>
#include <Server.hpp>
#include <fcntl.h>
#include <algorithm>

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

Response handleRequest(Request &request, vector<ConfigBlock> locations) {
	Response response;
	const ConfigBlock *location;
	string path;
	string target = request.getTarget();

	stable_sort(locations.begin(), locations.end(), compare);
	normalizeTarget(target);
	location = findLocation(locations, target);
	if (!location)
		return (Response(404));
	path = location->root + "/" + target.substr(location->prefix.size());
	switch (request.getMethodEnum())
	{
		case GET:
			response = handleGet(request, path, *location); break;
		case POST:
			response = handlePost(request, path, location->prefix); break;
		// case PUT:
		// 	response = handlePut(request);
		// case DELETE:
		// 	response = handleDelete(request);

		default:
			response = Response(501);
	}
	return (response);
}

Response processRequest(Request &request, const ConfigBlock &server) {
	int invalid;

	invalid = validateRequest(request, server);
	if (invalid)
		return (Response(invalid));
	
	// ResolvedContext ctx = resolveRequest(request, server);
	// if (ctx.invalid)
	// 	return (Response(invalid));
	// pass the ctx to the handlers
	return (handleRequest(request, server.locations));
}