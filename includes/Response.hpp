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
		Response(int c);
		
		// Setters
		void setHeader(string& key, string& value);
		void setBody(const string& b);
		
		// Getters
		int getCode() const;
		const string& getMessage() const;
		const map<string, string>& getHeaders() const;
		const string& getBody() const;
}; 

#endif