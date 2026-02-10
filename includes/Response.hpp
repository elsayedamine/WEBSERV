#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <ConfigBlock.hpp>

class Request;

class Response {
	private: 
		int code;
		int ready;
		std::map<std::string, std::string> headers;   
		std::vector<std::string> setCookieHeaders;
		std::string body;
		ConfigBlock server;

	public:
		// Constructor
		Response() : code (200), ready(1) {}
		Response(int c) : code(c), ready(1) {}
	
		// Setters
		void setHeader(const std::string& key, const std::string& value);
		void setBody(const std::string& b);
		void setStatus(int);
		void setReady(int);
		void setServer(const ConfigBlock &server);
		
		// Getters
		int isReady() const;
		int getCode() const;
		const std::map<std::string, std::string>& getHeaders() const;
		const std::vector<std::string>& getSetCookieHeaders() const;
		const std::string& getBody() const;
		std::string getHeader(const std::string& key) const;
		const ConfigBlock &getServer() const;
		
		std::string mkResponse();
		void process(const Request &request);
		std::string getCodeMessage(int code) const;
}; 

#endif