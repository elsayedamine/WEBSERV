#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <vector>
#include <iostream>
#include <map>

using namespace std;

class Request {  
	public:  
		enum Method {GET, DELETE, POST, PUT, UNKNOWN = -1}; 
																
	private: 
		Method method;
		string target;
		string version;
		map<string, string> headers; 
		string body; 
																
	public:
		int headerCount;
		Request(Method m, const string& t, const string& v);
		void setHeader(const string& key, const string& value);
		void setBody(const string& b);
		Method getMethod() const;
		const string& getTarget() const;
		const string& getVersion() const;
		const map<string, string>& getHeaders() const;
		string getHeader(const string& key) const;
		const string& getBody() const;
};

ostream& operator<<(ostream& os, const Request& req);

#endif // REQUEST_HPP
