#include "main.hpp"

int validateHeader(string key, string value) {
	if (key.empty())
		return (0);
	for (int i = 0; i < key.length(); i++) {
		if (!isalnum(key[i]) && key[i] != '-')
			return (0);
	}
	for (int i = 0; i < value.length(); i++) {
		if (value[i] < 32 || value[i] > 126)
			return (0);
	}
	return (1);
}

Response *validateRequest(Request &request) {
	{ // Request line
		if (request.getTarget().empty() || request.getVersion().empty())
			return (new Response(400, "Bad request"));
		if (request.getMethod() == -1)
			return (new Response(405, "Method Not Allowed"));
	}
	{ // Headers
		map<string, string> headers = request.getHeaders();

		for (map<string, string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
			if (!validateHeader(it->first, it->second))
				return (new Response(400, "Bad request"));				
		}
		if (request.getHeader("Host").empty())
			return (new Response(400, "Bad request"));
		if (request.getMethod() >= 2 && request.getHeader("Content-Length").empty())
			return (new Response(400, "Bad request"));
	}
	{ // Body
		if (request.getBody().length() != stoi(request.getHeader("Content-Length")))
			return (new Response(400, "Bad request"));
	}
	return (NULL);
}

Response *processRequest(Request &request) {
	Response *response;

	{ // Validate request
		response = validateRequest(request);
		// If this function returns a response, it's an error reponse to be
		// Sent immediately. Otherwise, if it's null, continue processing request
		if (response)
			return (response);
	}
	{ // Actually process the request

	}
	return (NULL);
}