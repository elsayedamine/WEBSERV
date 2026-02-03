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
		int data_ready;
		
	public:
		size_t offset;
		Parser parse;
		Request request;
		Response response;

		Connection() : fd(-1), data_ready(0), offset(0) { response.setReady(0); request.setReady(false); };
		Connection(int fd) : fd(fd), data_ready(0), offset(0) { response.setReady(0); request.setReady(false); };
		~Connection() {};

		int getFD() const { return fd; }
		void setFD(int fd) { this->fd = fd; }
		void setServers(const std::vector<ConfigBlock> &s) { servers = s; }
		const std::vector<ConfigBlock> &getServers() const { return servers; }
		const std::string &getData() const { return data; }
		void setData(const std::string &d) { data = d; }
		int dataReady() const { return data_ready; }
		void setDataReady(int r) { data_ready = (r != 0); }
		
		void read();
		void write();
		void processRequest();
		void processResponse();
		void reset();
};

#endif