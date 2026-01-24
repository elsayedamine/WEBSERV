#ifndef CONNECTIONS_HPP
#define CONNECTIONS_HPP

#include <iostream>
#include <Request.hpp>
#include <Response.hpp>
#include <ConfigBlock.hpp>
#include <Parse.hpp>

class Connection {
	private:
		int fd;
		ConfigBlock server;
		
	public:
		Parse parse;
		Request request;
		Response response;

		Connection() {};
		Connection(int fd) : fd(fd) {};
		~Connection() {};

		int getFD() const { return fd; }
		void setFD(int fd) { this->fd = fd; }
		const ConfigBlock &getServer() const { return server; }
};

#endif