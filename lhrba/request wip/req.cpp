#include "main.hpp"
#include "Request.hpp"

Request::Method stringToMethod(const string& methodStr) {
	if (methodStr == "GET") return Request::GET;
	if (methodStr == "POST") return Request::POST;
	if (methodStr == "PUT") return Request::PUT;
	if (methodStr == "DELETE") return Request::DELETE;
	return Request::UNKNOWN;
}

vector<string> tokenize(stringstream stream) {
	string token;
	vector<string> tokens;

	while (getline(stream, token, ' ')) {
		if (token.empty())
			return (tokens);
		tokens.push_back(token);
	}
	return tokens;
}

pair<string, string> parseHeader(stringstream stream) {
	string temp;
	pair<string, string> pair;

	getline(stream, temp, ':');
	if (temp.front() == ' ' || temp.back() == ' ')
		return (make_pair("error", "error"));
	pair.first = temp;
	stream >> temp;
	if (temp.empty())
		return (make_pair("error", "error"));
	pair.second = temp;

	return (pair);
}

Request *parseRequest(stringstream stream) {
	int cr = 0;
	Request *request;
	
	{ // Request line
		string reqline;
		vector<string> tokens;

		getline(stream, reqline);
		if (reqline.back() == '\r') {
			cr = 1;
			reqline.pop_back();
		}
		tokens = tokenize(stringstream(reqline));
		if (tokens.size() != 3)
			return (new Request(Request::UNKNOWN, "", ""));
		request = new Request(stringToMethod(tokens[0]), tokens[1], tokens[2]);
	}
	{ // Headers
		string header;
		pair<string, string> pair;
		while (1) {
			getline(stream, header);
			if ((header.back() == '\r') != cr) {
				request->setHeader("error", "error");
				return (request);
			}
			header.pop_back();
			pair = parseHeader(stringstream(header));
			if (!request->getHeader(pair.first).empty()) {
				request->setHeader("error", "error");
				return (request);
			}
			request->setHeader(pair.first, pair.second);
		}
	}
	return (request);
}

void handleConnection(int fd) {
	string data;
	char buffer[1024];
	Request *request;
	int error = 0;

	{
		ssize_t readSize;
		while ((readSize = read(fd, buffer, 1024)) > 0)
			data.append(buffer, readSize);
	}
	request = parseRequest(stringstream(data));
}