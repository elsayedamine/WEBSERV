#include "main.hpp"
#include "Request.hpp"

Request::Method stringToMethod(const string& methodStr) {
	if (methodStr == "GET") return Request::GET;
	if (methodStr == "POST") return Request::POST;
	if (methodStr == "PUT") return Request::PUT;
	if (methodStr == "DELETE") return Request::DELETE;
	return Request::UNKNOWN;
}

vector<string> tokenize(const string& line) {
	stringstream stream(line);
	string token;
	vector<string> tokens;

	while (stream >> token) {
		tokens.push_back(token);
	}

	return tokens;
}

string readline(int fd, bool includeDelimiter) {
	string line;
	char c;
	ssize_t bytes_read;
	
	while ((bytes_read = read(fd, &c, 1)) > 0) {
		if (c == '\0') {
			// Null terminator - end of line
			if (includeDelimiter) {
				line += c;
			}
			break;
		} else if (c == '\r') {
			char next_c;
			ssize_t next_read = read(fd, &next_c, 1);
			if (next_read > 0 && next_c == '\n') {
				// Found proper \r\n sequence
				if (includeDelimiter) {
					line += c;
					line += next_c;
				}
				break;
			} else {
				// \r not followed by \n - continue reading
				line += c;
				if (next_read > 0) {
					line += next_c;
				}
			}
		} else {
			line += c;
		}
	}
	return line;
}

Request *parseRequestLine(string line) {
	vector<string> tokens;
	
	tokens = tokenize(line);
	if (tokens.size() != 3)
		return (new Request(Request::UNKNOWN, "", ""));
	return (new Request(stringToMethod(tokens[0]), tokens[1], tokens[2]));
}

pair<string, string> parseHeader(string line) {
	stringstream stream(line);
	string temp;
	pair<string, string> en;

	getline(stream, temp, ':');
	en.first = temp;
	stream >> temp;
	en.second = temp;

	return (en);
}

void parseRequest(int fd) {
	string reqline;
	string body;
	string temp;
	pair<string, string> en;
	Request *request;
	
	reqline = readline(fd, false);
    request = parseRequestLine(reqline);
	while (1) {
		temp = readline(fd, 0);
		if (temp.empty())
			break;
		en = parseHeader(temp);
		request->setHeader(en.first, en.second);
	}
	//readline(fd, 1);
	while (1) {
		temp = readline(fd, 1);
		if (temp.empty())
			break;
		body += temp;
	}
	request->setBody(body);
	cout << *request;
}
