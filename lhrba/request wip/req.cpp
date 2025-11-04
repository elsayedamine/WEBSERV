#include "main.hpp"

Request::Method stringToMethod(const string& methodStr) {
    if (methodStr == "GET") return Request::GET;
    if (methodStr == "POST") return Request::POST;
    if (methodStr == "PUT") return Request::PUT;
    if (methodStr == "DELETE") return Request::DELETE;
    return Request::UNKNOWN;
}

Request parseRequestLine(string line) {
    stringstream stream(line);
    string token;
    vector<string> tokens;

    while (stream >> token) {
        tokens.push_back(token);
    }

    cout << tokens.size() << endl;

    return (Request(stringToMethod(tokens[0]), tokens[1]));
}

void parseRequest(int fd) {
	string reqline;
	char c;
	ssize_t bytes_read;
	
	while ((bytes_read = read(fd, &c, 1)) > 0) {
		if (c == '\r' || c == '\n') {
			break;
		}
		reqline += c;
	}
    cout << parseRequestLine(reqline);
}

