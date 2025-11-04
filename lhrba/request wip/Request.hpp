#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <cstring>
#include <unistd.h>

using namespace std;

class Request {  
	public:  
		enum Method {GET, POST, PUT, DELETE, UNKNOWN}; 
																
	private: 
		Method method;
		string target;
		map<string, string> headers; 
		string body; 
																
	public:  
		Request(Method m, const string& t);
		void setHeader(string& key, string& value);
		void setBody(const string& b);
		Method getMethod() const;
		const string& gettarget() const;
		const map<string, string>& getHeaders() const;
		string getHeader(const string& key) const;
		const string& getBody() const;
};

ostream& operator<<(ostream& os, const Request& req);

#endif // REQUEST_HPP
