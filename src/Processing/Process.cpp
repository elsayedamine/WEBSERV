#include <main.hpp>
#include <Methods.hpp>
#include <Server.hpp>
#include <fcntl.h>
#include <algorithm>

// Response *handlePut(Request &request) {
// 	(void)request;
// 	return (NULL);
// }

// Response *handleDelete(Request &request) {
// 	(void)request;
// 	return (NULL);
// }

Response handleRequest(Request &request, const ConfigBlock &server) {
	Response response;

	switch (request.getMethodEnum())
	{
		case GET:
			response = handleGet(request, server); break;
		case POST:
			response = handlePost(request, server); break;
		// case PUT:
		// 	response = handlePut(request);
		// case DELETE:
		// 	response = handleDelete(request);

		default:
			response = Response(501);
	}
	return (response);
}

struct ResolvedContext {
	const ConfigBlock* location;
	std::string path;
	int invalid;
	ResolvedContext(): invalid(0) {};
};

ResolvedContext resolveRequest(const Request &request, const ConfigBlock &server)
{
	ResolvedContext ctx;

	std::vector<ConfigBlock> locations = server.locations;
	std::stable_sort(locations.begin(), locations.end(), compare);

	std::string target = request.getTarget();
	normalizeTarget(target);

	const ConfigBlock* location = findLocation(locations, target);
	if (!location) {
		ctx.location = NULL;
		ctx.path = "";
		ctx.invalid = 404;
		return ctx; // should handle 404
	}
	ctx.location = location;

	ctx.path = location->root + target.substr(location->prefix.size());

	// we should not go outside the root
	
	return ctx;
}

Response processRequest(Request &request, const ConfigBlock &server) {
	int invalid;

	invalid = validateRequest(request, server);
	if (invalid)
		return (Response(invalid));
	
	ResolvedContext ctx = resolveRequest(request, server);
	if (ctx.invalid)
		return (Response(invalid));
	// pass the ctx to the handlers
	return (handleRequest(request, server));
}