#include <fcntl.h>
#include <main.hpp>

const string getMimeType(const string &file) {
	static map<string, string> types;
	if (types.empty()) {
		types["html"] =  "text/html";
		types["htm"] =   "text/html";
		types["css"] =   "text/css";
		types["js"] =    "application/javascript";
		types["mjs"] =   "application/javascript";
		types["json"] =  "application/json";
		types["txt"] =   "text/plain";
		types["xml"] =   "application/xml";
		types["jpg"] =   "image/jpeg";
		types["jpeg"] =  "image/jpeg";
		types["png"] =   "image/png";
		types["gif"] =   "image/gif";
		types["svg"] =   "image/svg+xml";
		types["ico"] =   "image/vnd.microsoft.icon";
		types["bmp"] =   "image/bmp";
		types["webp"] =  "image/webp";
		types["mp4"] =   "video/mp4";
		types["mpeg"] =  "video/mpeg";
		types["mp3"] =   "audio/mpeg";
		types["wav"] =   "audio/wav";
		types["ogg"] =   "audio/ogg";
		types["pdf"] =   "application/pdf";
		types["zip"] =   "application/zip";
		types["tar"] =   "application/x-tar";
		types["gz"] =    "application/gzip";
		types["7z"] =    "application/x-7z-compressed";
		types["csv"] =   "text/csv";
		types["woff"] =  "font/woff";
		types["woff2"] = "font/woff2";
		types["ttf"] =   "font/ttf";
	}
	
	{
		size_t dot = file.find_last_of('.');

		if (dot == file.npos)
			return ("application/octet-stream");
		string ext = file.substr(dot + 1);
		if (types.find(ext) == types.end())
			return ("application/octet-stream");
		return (types[ext]);
	}
}

int findResource(const vector<ConfigBlock> locations, const string &resource) {
	if (!access(resource.c_str(), F_OK))
		return (open(resource.c_str(), O_RDONLY));
	for (vector<ConfigBlock>::iterator it; it != locations.end(); ++it) {
		string path = it->prefix + resource;
		if (!access(path.c_str(), F_OK))
			return (open(path.c_str(), O_RDONLY));
	}
	return (-1);
}

Response *handleGet(Request &request, const ConfigBlock &server) {
	int fd;

	fd = findResource(server.locations, request.getTarget());
	if (fd == -1) {
		if (errno == EACCES)
			return (new Response(403));
		return (new Response(404));
	}
	{
		char buffer[1024];
		ssize_t readSize;
		string body;
		Response *response;

		while ((readSize = read(fd, buffer, 1024)) > 0)
			body.append(buffer, readSize);
		response = new Response(200);
		response->setBody(body);
		response->setHeader("Content-Type", getMimeType(request.getTarget()));
		response->setHeader("Content-Length", num_to_string(response->getBody().size()));
		return (response);
	}
}
