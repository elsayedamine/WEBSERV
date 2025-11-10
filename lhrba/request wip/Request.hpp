#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <vector>
#include <iostream>
#include <map>

using namespace std;

class Request {																
	private: 
		string method;
		string target;
		string version;
		multimap<string, string> headers; 
		string body; 
																
	public:
		int headerCount;
		Request(const string &m, const string &t, const string &v);
		void setHeader(const string &key, const string &value);
		void setBody(const string &b);
		const string &getMethod() const;
		const string &getTarget() const;
		const string &getVersion() const;
		const multimap<string, string> &getHeaders() const;
		string getHeader(const string &key) const;
		const string &getBody() const;
};

ostream &operator<<(ostream &os, const Request &req);

#endif // REQUEST_HPP
