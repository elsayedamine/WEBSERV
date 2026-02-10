#include <Methods.hpp>
#include <Server.hpp>
#include <fcntl.h>
#include <algorithm>
#include <sstream>
#include <iomanip>

Response handleReturn(const std::pair<int, std::string> &ret) {
	Response response(ret.first);

	if (ret.second.empty())
		return (response);
	response.setBody(ret.second);
	if (ret.first % 300 <= 99)
		response.setHeader("Location", ret.second);
	return (response);
}

int Request::process(Response &response) {
	const ConfigBlock *location;
	std::string path;

	{ // Resolve path
		// stable_sort(server.locations.begin(), server.locations.end(), compare);
		normalizeTarget(target);
		location = findLocation(server.locations, target);
		if (!location)
			return (response = Response(404), 0);
		if (location->root.empty())
			return (response = Response(500), 0);
		if (find(location->methods.begin(), location->methods.end(), getMethod()) == location->methods.end())
			return (response = Response(405), 0);
		if (location->ret.first)
			return (response = handleReturn(location->ret), 0);
		path = location->root + "/" + target.substr(location->prefix.size());
	}
	if (!location->cgi.empty()) {
		size_t pos = path.find_last_of('.');
		if (pos != std::string::npos) {
			for (map_it it = location->cgi.begin(); it != location->cgi.end(); ++it) {
				if (!path.compare(pos, it->first.size(), it->first)) {
					struct stat st;

					if (stat(path.c_str(), &st) == -1)
						return (response = Response(402 + (errno == EACCES) + 2 * (errno == ENOENT)), 0);
					if (S_ISDIR(st.st_mode))
						return (response = Response(403), 0);
					cgi.handleCGI(*this, path, it->second);
					return (1);
				}
			}
		}
	}
	switch (getMethodEnum()) {
		case GET:
			response = handleGet(path, *location); break;
		case POST:
			response = handlePost(*this, path, *location); break;
		case PUT:
			response = handlePut(*this, path); break;
		case DELETE:
			response = handleDelete(path); break;
		case UNKNOWN:
			response = Response(501);
	}
	return (0);
}

std::string dateTimeGMT() {
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

std::string mkErrorPage(int code, std::string message) {
	std::string body = std::string(
		"<!DOCTYPE html>"
		"<html lang=\"en\">"
		"<head>"
		"<meta charset=\"UTF-8\" />"
		"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />"
		"<script src=\"https://cdn.tailwindcss.com\"></script>"
		"<title>Error</title>"
		"</head>"
		"<body class=\"bg-gray-200 text-center\">"
		"<div class=\"mx-auto min-h-screen max-w-3xl bg-blue-100 py-10 border-l-4 border-r-4 border-black px-6\">"
		"<h1 class=\"font-bold text-8xl\">") + num_to_string(code) + std::string("</h1>"
		"<h1 class=\"font-bold text-5xl my-8\">") + message + std::string(
		"</h1></div></body></html>"
	);

	return (body);
}

void Response::process(const Request &request) {
	if (code >= 400) { // Check for error pages
		std::map<int, std::string>::const_iterator it = server.error_page.find(code);

		if (it != server.error_page.end())
			setBody(getResource(it->second).first);
		else
			setBody(mkErrorPage(code, getCodeMessage(code)));
		setHeader("Content-Type", "text/html");
	}
	{ // Finalize response
		setHeader("Connection", request.getHeader("Connection"));
		setHeader("Date", dateTimeGMT());
		setHeader("Server", "WEBSERV");
		if (!body.empty())
			setHeader("Content-Length", num_to_string(body.size()));
	}
	ready = true;
}
