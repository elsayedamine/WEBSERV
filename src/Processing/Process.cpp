#include <main.hpp>
#include <Methods.hpp>
#include <Server.hpp>
#include <fcntl.h>
#include <algorithm>

Response handleRedirect(const pair<int, string> &redirect) {
	Response response(redirect.first);

	response.setHeader("Location", redirect.second);
	return (response);
}

Response handleRequest(Request &request, vector<ConfigBlock> locations) {
	Response response;
	const ConfigBlock *location;
	string path;
	
	{ // Resolve path
		string target = request.getTarget();

		stable_sort(locations.begin(), locations.end(), compare);
		normalizeTarget(target);
		location = findLocation(locations, target);
		if (!location)
			return (Response(404));
		if (location->redirect.first)
			return (handleRedirect(location->redirect));
		path = location->root + "/" + target.substr(location->prefix.size());
	}
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
	return (handleRequest(request, server.locations));
}