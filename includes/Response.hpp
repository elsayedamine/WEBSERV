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

	public:
		// Constructor
		Response() : code (200), ready(1) {}
		Response(int c) : code(c), ready(1) {}
	
		// Setters
		void setHeader(const std::string& key, const std::string& value);
		void setBody(const std::string& b);
		void setStatus(int);
		void setReady(int);
		
		// Getters
		int isReady() const;
		int getCode() const;
		const std::map<std::string, std::string>& getHeaders() const;
		const std::string& getBody() const;
		std::string getHeader(const std::string& key) const;
		
		std::string mkResponse();
		void processResponse(const Request &request, const ConfigBlock &server);
		std::string getCodeMessage(int code) const;
}; 

#endif