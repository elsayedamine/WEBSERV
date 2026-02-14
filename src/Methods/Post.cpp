#include <algorithm>
#include <Methods.hpp>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <Utils.hpp>

const std::string getExtension(const std::string &type) {
	static std::map<std::string, std::string> types;
	if (types.empty()) {
		types["text/html"] = "html";
		types["text/css"] = "css";
		types["application/javascript"] = "js";
		types["application/json"] = "json";
		types["text/plain"] = "txt";
		types["application/xml"] = "xml";
		types["image/jpg"] = "jpg";
		types["image/png"] = "png";
		types["image/gif"] = "gif";
		types["image/svg+xml"] = "svg";
		types["image/vnd.microsoft.icon"] = "ico";
		types["image/bmp"] = "bmp";
		types["image/webp"] = "webp";
		types["video/mp4"] = "mp4";
		types["video/mpeg"] = "mpeg";
		types["audio/mpeg"] = "mp3";
		types["audio/wav"] = "wav";
		types["audio/ogg"] = "ogg";
		types["application/pdf"] = "pdf";
		types["application/zip"] = "zip";
		types["application/x-tar"] = "tar";
		types["application/gzip"] = "gz";
		types["application/x-7z-compressed"] = "7z";
		types["text/csv"] = "csv";
		types["font/woff"] = "woff";
		types["font/woff2"] = "woff2";
		types["font/ttf"] = "ttf";
	}

	if (types.find(type) == types.end())
		return ("");
	return ('.' + types[type]);
}

std::string getFilename(const std::string &path) {
	std::string last_file;
	int file_num;

	{ // Find most recent file
		DIR *d = opendir(path.c_str());
		struct dirent *ent;
		time_t most_recent = 0;
	
		while ((ent = readdir(d)) != NULL) {
			std::string curr = path + '/' + ent->d_name;
			struct stat st;
	
			if (ent->d_name[0] == '.') continue;
			if (stat(curr.c_str(), &st) != 0) continue;
			if (last_file.empty() || st.st_mtime > most_recent) {
				last_file = ent->d_name;
				most_recent = st.st_mtime;
			}
		}
		closedir(d);
	}
	{ // Turn filename into int
		size_t end;
		std::string name;
		
		if (last_file.empty())
			file_num = 0;
		else {
			end = last_file.find_first_of('.');
			if (end == std::string::npos)
				end = last_file.size();
			name = last_file.substr(0, end);
			file_num = stringToInt(name);
		}
	}
	return (num_to_string(file_num + 1));
}

std::string createResource(const std::string &path, const Request &request, const std::string &prefix) {
	std::string filepath;
	std::string filename;
	std::string uri = prefix;

	filename = getFilename(path) + getExtension(request.getHeader("Content-Type"));
	filepath = path + '/' + filename;
	int fd = open(filepath.c_str(), O_WRONLY | O_CREAT, 0644);
	if (fd == -1)
		return ("");
	write(fd, request.getBody().c_str(), request.getBody().size());
	close(fd);
	if (uri[uri.size() - 1] != '/')
		uri += '/';
	uri += filename;
	return (uri);
}

Response handlePost(const Request &request, const std::string &path, const ConfigBlock &location) {
	std::string name;
	struct stat st;

	if (!location.upload_enable)
		return (Response(403));
	stat(path.c_str(), &st);
	if (!S_ISDIR(st.st_mode))
		return (Response(403));
	name = createResource(path, request, location.prefix);
	{ // Form response
		Response response(201);

		// response.setBody(name);
		// response.setHeader("Content-Type", "text/plain");
		response.setHeader("Location", name);
		return (response);
	}
}
