#include <Methods.hpp>
#include <Utils.hpp>

std::string getCodeMessage(int code) {
	static map<int, std::string> messages;
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

std::string buildResponse(Response &response) {
	std::string data;

	{ // Status line
		data = "HTTP/1.1 ";
		data += num_to_string(response.getCode()) + ' ';
		data += getCodeMessage(response.getCode()) + "\r\n";
	}
	{ // Headers
		map<std::string, std::string> headers = response.getHeaders();

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

const std::string getMimeType(const std::string &file) {
	static map<std::string, std::string> types;
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
		std::string ext = file.substr(dot + 1);
		if (types.find(ext) == types.end())
			return ("application/octet-stream");
		return (types[ext]);
	}
}
