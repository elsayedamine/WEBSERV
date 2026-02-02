#ifndef CONNECTIONS_HPP
#define CONNECTIONS_HPP

#include <iostream>
#include <Request.hpp>
#include <Response.hpp>
#include <ConfigBlock.hpp>
#include <Parser.hpp>
#include <CGI.hpp>

class Connection {
	private:
		ConfigBlock server;
		int fd;
		
	public:
		Parser parse;
		Request request;
		Response response;
		CGI cgi;

		Connection() {};
		Connection(int fd) : fd(fd) {};
		~Connection() {};

		int getFD() const { return fd; }
		void setFD(int fd) { this->fd = fd; }
		const ConfigBlock &getServer() const { return server; }
};

#endif