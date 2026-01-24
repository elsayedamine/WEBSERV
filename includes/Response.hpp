#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>
#include <iostream>

class Request;
class ConfigBlock;

class Response {
	private: 
		int code;
		std::map<std::string, std::string> headers;   
		std::string body;
		int ready;
		std::string data;

	public:
		// Constructor
		Response() : ready(0) {}
		Response(int c) : code(c), ready(0) {}
		
		// Setters
		void setHeader(const std::string& key, const std::string& value);
		void setBody(const std::string& b);
		void setData(const std::string& d);
		
		// Getters
		int isReady() const;
		int getCode() const;
		const std::map<std::string, std::string>& getHeaders() const;
		const std::string& getBody() const;
		std::string getHeader(const std::string& key) const;
		const std::string& getData() const;
		
		void mkResponse();
		void processResponse(const Request &request, const ConfigBlock &server);
		std::string getCodeMessage(int code) const;
}; 

#endif