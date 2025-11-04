#include "Request.hpp"

// Request class method implementations
Request::Request(Method m, const string& t) {
    this->method = m;
    this->target = t;
}

void Request::setHeader(string& key, string& value) {
    headers[key] = value;
}

void Request::setBody(const string& b) { 
    body = b;
}

Request::Method Request::getMethod() const { 
    return method; 
}

const string& Request::gettarget() const { 
    return target; 
}

const map<string, string>& Request::getHeaders() const { 
    return headers; 
}

string Request::getHeader(const string& key) const {  
    map<string, string>::const_iterator it = headers.find(key); 
    return it != headers.end() ? it->second : "";
}

const string& Request::getBody() const { 
    return body; 
}

// Operator<< overload implementation
ostream& operator<<(ostream& os, const Request& req) {
    // Print method as a string
    string methodStr;
    switch (req.getMethod()) {
        case Request::GET:     methodStr = "GET"; break;
        case Request::POST:    methodStr = "POST"; break;
        case Request::PUT:     methodStr = "PUT"; break;
        case Request::DELETE:  methodStr = "DELETE"; break;
        default:               methodStr = "UNKNOWN"; break;
    }

    os << "=== Request Debug Info ===\n";
    os << "Method: " << methodStr << "\n";
    os << "Target: " << req.gettarget() << "\n";
    os << "Headers:\n";

    // Use traditional iterator for compatibility
    for (map<string, string>::const_iterator it = req.getHeaders().begin(); 
         it != req.getHeaders().end(); ++it)
        os << "  " << it->first << ": " << it->second << "\n";

    os << "Body:\n" << req.getBody() << "\n";
    os << "==========================\n";

    return os;
}
