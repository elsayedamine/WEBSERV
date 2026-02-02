#include <Methods.hpp>
#include <Utils.hpp>

Response handleDelete(const std::string &path)
{
	Response resp;

	if (unlink(path.c_str()) == 0)
	{
		resp.setStatus(204);
		resp.setHeader("Content-Length", "0");
		return resp;
	}

	// check (error message and code are in the header, not body)
	switch (errno)
	{
		case ENOENT:
			resp.setStatus(404);
			resp.setBody("404 Not Found");
			break;

		case EISDIR:
			resp.setStatus(405);
			resp.setBody("405 Method Not Allowed");
			break;

		case EACCES:
			resp.setStatus(403);
			resp.setBody("403 Forbidden");
			break;

		default:
			resp.setStatus(500);
			resp.setBody("500 Internal Server Error");
			break;
	}

	resp.setHeader("Content-Type", "text/plain");
	resp.setHeader("Content-Length", num_to_string(resp.getBody().size()));

	return resp;
}
