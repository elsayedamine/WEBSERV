#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>
#include <iostream>

class Request;
struct ConfigBlock;

class Response {
	private: 
		int code;
		int ready;
		std::map<std::string, std::string> headers;   
		std::string body;
		std::string data;

	public:
		// Constructor
		Response() : code (200), ready(0) {}
		Response(int c) : code(c), ready(0) {}
		
		// Setters
		void setHeader(const std::string& key, const std::string& value);
		void setBody(const std::string& b);
		void setData(const std::string& d);
		void setStatus(int);
		
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