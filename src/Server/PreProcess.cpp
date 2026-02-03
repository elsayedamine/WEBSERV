#include <Utils.hpp>

#define MAX_BODY 10 * 1000 * 1000
#define MAX_URI 10 * 1000 * 1000

int validateHeader(std::string key, std::string value) {
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

int checkVersion(const std::string &ver) {
	if (ver.length() != 8 || ver.compare(0, 5, "HTTP/"))
		return (400);
	if (ver.compare(5, 3, "1.1"))
		return (505);
	return (0);
}

int Request::validateRequest() const {
	{ // Request line
		int validVer;

		if (target.empty() || method.empty())
			return (400);
		if (target.length() > MAX_URI)
			return (414);
		validVer = checkVersion(version);
		if (validVer)
			return (validVer);
	}
	{ // Headers
		for (mmap_it it = headers.begin(); it != headers.end(); ++it) {
			std::pair<mmap_it, mmap_it> range = headers.equal_range(it->first);
			if (distance(range.first, range.second) > 1)
				return (400);
		}
		if (getHeader("Host").empty())
			return (400);
	}
	{ // Body
		if (getHeader("Content-Length").empty()) {
			if (method == "POST" || method == "PUT")
				return (411);
			return (0);
		}
		// check this line may be wrong later
		if ((long)body.length() > MAX_BODY)
			return (413);
		if (body.length() != (size_t)stringToInt(getHeader("Content-Length")))
			return (400);
	}
	return (0);
}
