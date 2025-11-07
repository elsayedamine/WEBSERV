#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>   
#include <iostream>  
 
using namespace std; 
 
class Response { 
	private: 
		int code;
		string message;  
		map<string, string> headers;   
		string body; 

	public:  
		// Constructor   
		Response(int c, const string& m);
		
		// Setters
		void setHeader(string& key, string& value);
		void setBody(const string& b);
		
		// Getters
		int getCode() const;
		const string& getMessage() const;
		string getHeader(const string& key) const;
		const string& getBody() const;
}; 

#endif