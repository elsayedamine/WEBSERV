#include <main.hpp>
#include <Server.hpp>

// Helper to print string vectors
static std::ostream &printVector(std::ostream &os, const std::vector<std::string> &v) {
	os << "[";
	for (size_t i = 0; i < v.size(); ++i) {
		if (i) os << ", ";
		os << v[i];
	}
	os << "]";
	return os;
}

// Stream output for ConfigBlock for debugging
std::ostream &operator<<(std::ostream &os, const ConfigBlock &cb) {
	os << "ConfigBlock {\n";
	os << "  err: " << cb.err << "\n";
	os << "  port: " << cb.port << "\n";
	os << "  host: " << cb.host << "\n";
	os << "  server_name: "; printVector(os, cb.server_name); os << "\n";
	os << "  prefix: " << cb.prefix << "\n";
	os << "  cgi_path: "; printVector(os, cb.cgi_path); os << "\n";
	os << "  autoindex: " << cb.autoindex << "\n";
	os << "  upload_enable: " << cb.upload_enable << "\n";
	os << "  root: " << cb.root << "\n";
	os << "  upload_path: " << cb.upload_path << "\n";
	os << "  client_max_body_size: " << cb.client_max_body_size << "\n";
	os << "  error_page: {";
	{
		bool first = true;
		for (std::map<int, std::string>::const_iterator it = cb.error_page.begin(); it != cb.error_page.end(); ++it) {
			if (!first) os << ", ";
			os << it->first << ":" << it->second;
			first = false;
		}
	}
	os << "}\n";
	os << "  redirect: (" << cb.redirect.first << ", " << cb.redirect.second << ")\n";
	os << "  index: "; printVector(os, cb.index); os << "\n";
	os << "  methods: "; printVector(os, cb.methods); os << "\n";
	os << "  locations: [\n";
	for (size_t i = 0; i < cb.locations.size(); ++i) {
		os << "    " << cb.locations[i] << (i + 1 < cb.locations.size() ? ",\n" : "\n");
	}
	os << "  ]\n";
	os << "}\n";
	return os;
}

// Response *handleGet(Request &request) {
// 	(void)request;
// 	return (NULL);
// }

// Response *handlePost(Request &request) {
// 	(void)request;
// 	return (NULL);
// }

// Response *handlePut(Request &request) {
// 	(void)request;
// 	return (NULL);
// }

// Response *handleDelete(Request &request) {
// 	(void)request;
// 	return (NULL);
// }

// Response *handleRequest(Request &request) {
// 	Response *response;

// 	switch (request.getMethodEnum())
// 	{
// 		case GET:
// 			response = handleGet(request);
// 		case POST:
// 			response = handlePost(request);
// 		case PUT:
// 			response = handlePut(request);
// 		case DELETE:
// 			response = handleDelete(request);
// 		default:
// 			response = new Response(501);
// 	}
// 	return (response);
// }

Response *processRequest(Request &request, const ConfigBlock &server) {
	Response *response;

	{ // Validate request
		response = validateRequest(request);
		if (response)
			return (response);
	}
	{ // Actually process the request
		// response = handleRequest(request);
		cout << server << endl;
	}
	return (response);
}