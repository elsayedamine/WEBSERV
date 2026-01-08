#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>
#include <iostream>

using namespace std;
 
class Response {
	private: 
		int code;
		map<string, string> headers;   
		string body; 

	public:
		// Constructor
		Response() {}
		Response(int c) : code(c) {}
		
		// Setters
		void setHeader(const string& key, const string& value) { headers[key] = value; }
		void setBody(const string& b) { body = b; }
		
		// Getters
		int getCode() const { return code; }
		const map<string, string>& getHeaders() const { return headers; }
		const string& getBody() const { return body; }
}; 

#endif