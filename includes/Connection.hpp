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

#define TIMEOUT 5000

class Connection {
	private:
		std::vector<ConfigBlock> servers;
		ConfigBlock server;
		std::string data;
		int fd;
		
	public:
		Parser parse;
		size_t offset;
		Request request;
		Response response;
		int timeout;
		bool timed_out;

		Connection() : fd(-1), parse(servers), offset(0), timeout(TIMEOUT), timed_out(false) { response.setReady(0); request.setReady(false); };
		Connection(int fd) : fd(fd), parse(servers), offset(0), timeout(TIMEOUT), timed_out(false) { response.setReady(0); request.setReady(false); };
		~Connection() {};

		int getFD() const { return fd; }
		void setFD(int fd) { this->fd = fd; }
		void setServers(const std::vector<ConfigBlock> &s) { servers = s; parse = Parser(servers); }
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