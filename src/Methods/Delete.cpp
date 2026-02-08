#include <Methods.hpp>
#include <Utils.hpp>

Response handleDelete(const std::string &path)
{
	Response resp;

	if (unlink(path.c_str()) == 0)
	{
		resp.setStatus(204);
		return resp;
	}

	switch (errno)
	{
		case ENOENT:
			resp.setStatus(404);
			break;

		case EISDIR:
			resp.setStatus(405);
			break;

		case EACCES:
			resp.setStatus(403);
			break;

		default:
			resp.setStatus(500);
			break;
	}

	resp.setHeader("Content-Type", "text/plain");
	return resp;
}
