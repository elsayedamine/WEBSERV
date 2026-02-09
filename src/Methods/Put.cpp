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
	resp.setHeader("Location", req.getTarget());
	if (!existed)
	{
		resp.setStatus(201);
		resp.setHeader("Content-Type", "text/plain");
		return resp;
	}
	resp.setStatus(204);
	return resp;
}
