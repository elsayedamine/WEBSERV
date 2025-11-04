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
		if (c == '\r') {
			char next_c;
			ssize_t next_read = read(fd, &next_c, 1);
			if (next_read > 0 && next_c == '\n') {
				if (includeDelimiter) {
					line += c;
					line += next_c;
				}
				break;
			} else {
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
	string header;
	pair<string, string> en;
	Request *request;
	
	reqline = readline(fd, false);
    request = parseRequestLine(reqline);
	while (1) {
		header = readline(fd, 0);
		if (header.empty())
			break;
		en = parseHeader(header);
		request->setHeader(en.first, en.second);
	}
	cout << *request;
}
