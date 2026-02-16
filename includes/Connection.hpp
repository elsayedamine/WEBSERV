#ifndef CONNECTIONS_HPP
#define CONNECTIONS_HPP

#include <iostream>
#include <vector>
#include <string>
#include <Request.hpp>
#include <Response.hpp>
#include <ConfigBlock.hpp>
#include <Parser.hpp>
#include <CGI.hpp>

class Connection {
	private:
		std::vector<ConfigBlock> servers;
		ConfigBlock server;
		int fd;
		std::string data;
		
	public:
		size_t offset;
		Parser parse;
		Request request;
		Response response;

		Connection() : fd(-1), offset(0) { response.setReady(0); request.setReady(false); };
		Connection(int fd) : fd(fd), offset(0) { response.setReady(0); request.setReady(false); };
		~Connection() {};

		int getFD() const { return fd; }
		void setFD(int fd) { this->fd = fd; }
		void setServers(const std::vector<ConfigBlock> &s) { servers = s; }
		const std::vector<ConfigBlock> &getServers() const { return servers; }
		const std::string &getData() const { return data; }
		void setData(const std::string &d) { data = d; }
		
		void read();
		int write();
		void processRequest();
		void processResponse();
		void reset();
};

#endif