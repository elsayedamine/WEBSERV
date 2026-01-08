#include <main.hpp>
#include <Utils.hpp>

#define MAX_BODY 10 * 1000 * 1000
#define MAX_URI 10 * 1000 * 1000

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

int checkVersion(const string &ver) {
	if (ver.length() != 8 || ver.compare(0, 5, "HTTP/"))
		return (400);
	if (ver.compare(5, 3, "1.1"))
		return (505);
	return (0);
}

int validateRequest(Request &request, const ConfigBlock &server) {
	(void)server;
	{ // Request line
		int validVer;

		if (request.getTarget().empty() || request.getMethod().empty())
			return (400);
		if (request.getTarget().length() > MAX_URI)
			return (414);
		validVer = checkVersion(request.getVersion());
		if (validVer)
			return (validVer);
	}
	{ // Headers
		const multimap<string,string>& headers = request.getHeaders();

		if (request.headerCount == -1)
			return (400);
		for (mmap_it it = headers.begin(); it != headers.end(); ++it) {
			pair<mmap_it, mmap_it> range = headers.equal_range(it->first);
			if (distance(range.first, range.second) > 1)
				return (400);
		}
		if (request.getHeader("Host").empty())
			return (400);
	}
	{ // Body
		if (request.getHeader("Content-Length").empty()) {
			if (request.getMethod() == "POST" || request.getMethod() == "PUT")
				return (411);
			return (0);
		}
		// this line may be wrong later
		if ((long)request.getBody().length() > MAX_BODY)
			return (413);
		if (request.getBody().length() != (size_t)stringToInt(request.getHeader("Content-Length")))
			return (400);
	}
	return (0);
}
