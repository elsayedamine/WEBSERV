#include "main.hpp"

string getCodeMessage(int code) {
	static map<int, string> messages = {
		{200, "OK"},
		{201, "Created"},
		{204, "No Content"},
		{301, "Moved Permanently"},
		{302, "Found"},
		{304, "Not Modified"},
		{400, "Bad Request"},
		{403, "Forbidden"},
		{404, "Not Found"},
		{405, "Method Not Allowed"},
		{411, "Length Required"},
		{413, "Payload Too Large"},
		{414, "URI Too Long"},
		{415, "Unsupported Media Type"},
		{500, "Internal Server Error"},
		{501, "Not Implemented"},
		{502, "Bad Gateway"}
	};

	if (messages.find(code) == messages.end())
		return ("");
	return (messages[code]);
}

string buildResponse(Response &response) {
	string data;

	{ // Status line
		data = "HTTP/1.1 ";
		data += to_string(response.getCode()) + ' ';
		data += getCodeMessage(response.getCode()) + "\r\n";
	}
	{ // Headers
		for (map<string, string>::const_iterator it = response.getHeaders().begin(); 
			it != response.getHeaders().end(); ++it) {
			data += it->first + ": " + it->second;
			if (next(it) != response.getHeaders().end())
				data += "\r\n";
		}
	}
	{
		if (response.getBody().empty())
			return (data);
		data += "\r\n" + response.getBody();
	}
	return (data);
}

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
			return (new Response(400));
		if (request.getMethod() == -1)
			return (new Response(405));
	}
	{ // Headers
		map<string, string> headers = request.getHeaders();

		for (map<string, string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
			if (!validateHeader(it->first, it->second))
				return (new Response(400));
		}
		if (request.getHeader("Host").empty())
			return (new Response(400));
		if (request.getMethod() >= 2 && request.getHeader("Content-Length").empty())
			return (new Response(400));
	}
	{ // Body
		if (request.getBody().length() != stoi(request.getHeader("Content-Length")))
			return (new Response(400));
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