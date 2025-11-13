#include <main.hpp>

string intToString(int n) {
	std::ostringstream oss;
	oss << n;
	return oss.str();
}

int stringToInt(string str) {
	size_t n = 0;
	std::istringstream iss(str);
	iss >> n;
	return (n);
}

string getCodeMessage(int code) {
	static map<int, string> messages;
	if (messages.empty()) {
		messages[200] = "OK";
		messages[201] = "Created";
		messages[204] = "No Content";
		messages[301] = "Moved Permanently";
		messages[302] = "Found";
		messages[304] = "Not Modified";
		messages[400] = "Bad Request";
		messages[403] = "Forbidden";
		messages[404] = "Not Found";
		messages[405] = "Method Not Allowed";
		messages[411] = "Length Required";
		messages[413] = "Payload Too Large";
		messages[414] = "URI Too Long";
		messages[415] = "Unsupported Media Type";
		messages[500] = "Internal Server Error";
		messages[501] = "Not Implemented";
		messages[502] = "Bad Gateway";
	}

	if (messages.find(code) == messages.end())
		return ("");
	return (messages[code]);
}

string buildResponse(Response &response) {
	string data;

	{ // Status line
		data = "HTTP/1.1 ";
		data += intToString(response.getCode()) + ' ';
		data += getCodeMessage(response.getCode()) + "\r\n";
	}
	{ // Headers
		map<string, string> headers = response.getHeaders();

		map_it it = headers.begin();
		while (1) {
			data += it->first + ": " + it->second;
			++it;
			if (it == headers.end())
				break;
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
	for (int i = 0; (size_t)i < key.length(); i++) {
		if (!isalnum(key[i]) && key[i] != '-')
			return (0);
	}
	for (int i = 0; (size_t)i < value.length(); i++) {
		if (value[i] < 32 || value[i] > 126)
			return (0);
	}
	return (1);
}

Response *validateRequest(Request &request) {
	{ // Request line
		if (request.getTarget().empty() || request.getVersion().empty() || request.getMethod().empty())
			return (new Response(400));
	}
	{ // Headers
		const multimap<string,string>& headers = request.getHeaders();

		for (mmap_it it = headers.begin(); it != headers.end(); ++it) {
			pair<mmap_it, mmap_it> range = headers.equal_range(it->first);
			if (distance(range.first, range.second) > 1)
				return new Response(400);
		}
		if (request.getHeader("Host").empty())
			return (new Response(400));
	}
	{ // Body
		if (request.getHeader("Content-Length").empty()) {
			if (request.getMethod() == "POST" || request.getMethod() == "PUT")
				return (new Response(400));
			return (NULL);
		}
		if (request.getBody().length() != (size_t)stringToInt(request.getHeader("Content-Length")))
			return (new Response(400));
	}
	return (NULL);
}
