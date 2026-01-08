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
