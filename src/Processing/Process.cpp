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

Response Request::handleRequest(const vector<ConfigBlock> &locations) const {
	Response response;
	const ConfigBlock *location;
	string path;
	
	{ // Resolve path
		string target = target;

		stable_sort(locations.begin(), locations.end(), compare);
		normalizeTarget(target);
		location = findLocation(locations, target);
		if (!location)
			return (Response(404));
		if (location->root.empty())
			return (Response(500));
		if (find(location->methods.begin(), location->methods.end(), getMethod()) == location->methods.end())
			return (405);
		if (location->ret.first)
			return (handleReturn(location->ret));
		path = location->root + "/" + target.substr(location->prefix.size());
	}
	// if (cgi)
	//	connection.cgi.handleCGI(request, script, interpreter, fd);
	switch (getMethodEnum()) {
		case GET:
			response = handleGet(path, *location); break;
		case POST:
			response = handlePost(*this, path, *location); break;
		case PUT:
			response = handlePut(*this, path); break;
		case DELETE:
			response = handleDelete(path); break;
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

void Response::processResponse(const Request &request, const ConfigBlock &server) {
	if (code >= 400) { // Check for error pages
		map<int, string>::const_iterator it = server.error_page.find(code);

		if (it != server.error_page.end()) {
			setBody(getResource(it->second).first);
			setHeader("Content-Type", "text/html");
		}
	}
	{ // Finalize response
		setHeader("Connection", request.getHeader("Connection"));
		setHeader("Date", dateTimeGMT());
		setHeader("Server", "WEBSERV");
		if (!body.empty())
			setHeader("Content-Length", num_to_string(body.size()));
	}	
}

Response Request::processRequest() {
	int invalid;
	Response response;

	invalid = validateRequest();
	if (invalid)
		return (Response(invalid));

	response = handleRequest(server.locations);
	return (response);
}