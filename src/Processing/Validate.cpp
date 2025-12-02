#include <main.hpp>
#include <Utils.hpp>

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

int validateRequest(Request &request) {
	{ // Request line
		if (request.getTarget().empty() || request.getVersion().empty() || request.getMethod().empty())
			return (400);
	}
	{ // Headers
		const multimap<string,string>& headers = request.getHeaders();

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
				return (400);
			return (0);
		}
		if (request.getBody().length() != (size_t)stringToInt(request.getHeader("Content-Length")))
			return (400);
	}
	return (0);
}
