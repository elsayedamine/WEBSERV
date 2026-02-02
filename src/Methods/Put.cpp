#include <fcntl.h>
#include <sys/stat.h>
#include <Methods.hpp>

Response handlePut(const Request &req, const std::string &path)
{
	struct stat st;
	bool existed = (stat(path.c_str(), &st) == 0);

	if (existed && S_ISDIR(st.st_mode))
		return Response(403);

	std::ofstream file(path.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
	if (!file)
		return Response(403);
	file.write(req.getBody().data(), req.getBody().size());
	file.close();

	Response resp;
	if (!existed)
	{
		// check (you dont have to do allat)
		resp.setStatus(201);
		resp.setBody("201 Created");
		resp.setHeader("Content-Type", "text/plain");
		resp.setHeader("Content-Length", "11");
		return resp;
	}

	resp.setStatus(204);
	resp.setHeader("Content-Length", "0");
	return resp;
}
