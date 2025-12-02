#include <main.hpp>
#include <Server.hpp>
#include <fcntl.h>

// Response *handlePut(Request &request) {
// 	(void)request;
// 	return (NULL);
// }

// Response *handleDelete(Request &request) {
// 	(void)request;
// 	return (NULL);
// }

Response *handleRequest(Request &request, const ConfigBlock &server) {
	Response *response;

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
			response = new Response(501);
	}
	return (response);
}

Response *processRequest(Request &request, const ConfigBlock &server) {
	Response *response;

	response = validateRequest(request);
	if (response)
		return (response);
	return (handleRequest(request, server));
}