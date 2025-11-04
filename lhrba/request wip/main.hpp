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
		Request(Method m, const string& t) {
			this->method = m;
			this->target = t;
		}
		void setHeader(string& key, string& value) {
			headers[key] = value;
		}
		void setBody(const string& b) { body = b;}
		Method getMethod() const { return method; }  
		const string& gettarget() const { return target; }
		const map<string, string>& getHeaders() const { return headers; }
		string getHeader(const string& key) const {  
			map<string, string>::const_iterator  it = headers.find(key); 
			return it != headers.end() ? it->second : "";
		}
		const string& getBody() const { return body; }	
};

ostream& operator<<(ostream& os, const Request& req) {
    // Print method as a string
    string methodStr;
    switch (req.getMethod()) {
        case Request::GET:     methodStr = "GET"; break;
        case Request::POST:    methodStr = "POST"; break;
        case Request::PUT:     methodStr = "PUT"; break;
        case Request::DELETE:  methodStr = "DELETE"; break;
        default:                       methodStr = "UNKNOWN"; break;
    }

    os << "=== Request Debug Info ===\n";
    os << "Method: " << methodStr << "\n";
    os << "Target: " << req.gettarget() << "\n";
    os << "Headers:\n";

    // Use range-based for loop for headers
    for (map<string, string>::const_iterator it = req.getHeaders().begin(); 
         it != req.getHeaders().end(); ++it)
        os << "  " << it->first << ": " << it->second << "\n";

    os << "Body:\n" << req.getBody() << "\n";
    os << "==========================\n";

    return os;
}
