#include <iostream>
#include <Request.hpp>
#include <Response.hpp>
#include <ConfigBlock.hpp>

class Connection {
	private:
		int fd;
		string data;
		ConfigBlock server;
		
	public:
		Request request;
		Response response;

		Connection(int fd) : fd(fd) {};
		~Connection() {};

		int getFD() const { return fd; }
		const ConfigBlock &getServer() const { return server; }
};