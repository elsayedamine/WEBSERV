#include <main.hpp>
#include <Methods.hpp>
#include <Server.hpp>
#include <fcntl.h>
#include <algorithm>
#include <sstream>
#include <iomanip>

Response handleReturn(const pair<int, string> &ret) {
	Response response(ret.first);

	if (ret.second.empty())
		return (response);
	response.setBody(ret.second);
	if (ret.first % 300 <= 99)
		response.setHeader("Location", ret.second);
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
		if (find(location->methods.begin(), location->methods.end(), request.getMethod()) == location->methods.end())
			return (405);
		if (location->redirect.first)
			return (handleReturn(location->redirect));
		path = location->root + "/" + target.substr(location->prefix.size());
	}
	switch (request.getMethodEnum())
	{
		case GET:
			response = handleGet(request, path, *location); break;
		case POST:
			response = handlePost(request, path, *location); break;
		// case PUT:
		// 	response = handlePut(request);
		// case DELETE:
		// 	response = handleDelete(request);

		default:
			response = Response(501);
	}
	return (response);
}

int validateType(const string &target, const string &type) {
	if (target[0] == '*')
		return (1);
	if (target[target.size() - 1] == '*') {
		size_t t1 = target.find_first_of('/');
		size_t t2 = type.find_first_of('/');

		if (target.substr(0, t1) != type.substr(0, t2))
			return (0);
		return (1);
	}
	if (target != type)
		return (0);
	return (1);
}

string dateTimeGMT() {
	static const char* wkday[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	static const char* month[] = {"Jan","Feb","Mar","Apr","May","Jun",
								"Jul","Aug","Sep","Oct","Nov","Dec"};
	time_t t = time(NULL);
	struct tm gmt = *gmtime(&t);
	std::ostringstream oss;
	
	oss << wkday[gmt.tm_wday] << ", "
		<< std::setw(2) << std::setfill('0') << gmt.tm_mday << " "
		<< month[gmt.tm_mon] << " "
		<< (gmt.tm_year + 1900) << " "
		<< std::setw(2) << std::setfill('0') << gmt.tm_hour << ":"
		<< std::setw(2) << std::setfill('0') << gmt.tm_min << ":"
		<< std::setw(2) << std::setfill('0') << gmt.tm_sec << " GMT";
	return oss.str();
}

Response processResponse(const Request &request, Response &response, const ConfigBlock &server) {
	// if (!request.getHeader("Accept").empty()) {
	// 	if (!validateType(request.getHeader("Accept"), response.getHeader("Content-Type")))
	// 		response = Response(406);
	// }
	if (response.getCode() >= 400) { // Check for error pages
		map<int, string>::const_iterator it = server.error_page.find(response.getCode());

		cout << it->second << endl;
		if (it != server.error_page.end()) {
			response.setBody(getResource(it->second).first);
			response.setHeader("Content-Type", "text/html");
		}
	}
	{ // Finalize response
		response.setHeader("Connection", request.getHeader("Connection"));
		response.setHeader("Date", dateTimeGMT());
		response.setHeader("Server", "WEBSERV");
		if (!response.getBody().empty())
			response.setHeader("Content-Length", num_to_string(response.getBody().size()));
	}	
	return (response);
}

Response processRequest(Request &request, const ConfigBlock &server) {
	int invalid;
	Response response;

	invalid = validateRequest(request, server);
	if (invalid)
		return (Response(invalid));
	response = handleRequest(request, server.locations);
	return (processResponse(request, response, server));
}