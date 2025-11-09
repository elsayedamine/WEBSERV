#include "main.hpp"
#include "Request.hpp"

Request::Method stringToMethod(const string &methodStr) {
	if (methodStr == "GET") return Request::GET;
	if (methodStr == "POST") return Request::POST;
	if (methodStr == "PUT") return Request::PUT;
	if (methodStr == "DELETE") return Request::DELETE;
	return Request::UNKNOWN;
}

vector<string> tokenize(string &line) {
	stringstream stream(line);
	string token;
	vector<string> tokens;

	while (getline(stream, token, ' ')) {
		if (token.empty())
			return (tokens);
		tokens.push_back(token);
	}
	return tokens;
}

pair<string, string> parseHeader(string &header) {
	stringstream stream(header);
	string temp;
	pair<string, string> pair;

	getline(stream, temp, ':');
	if (temp.length() > 0 && (temp[0] == ' ' || temp[temp.length() - 1] == ' '))
		return (make_pair("", ""));
	pair.first = temp;
	stream >> temp;
	if (temp.empty())
		return (make_pair("", ""));
	pair.second = temp;

	return (pair);
}

Request *parseRequest(string &data) {
	stringstream stream(data);
	int cr = 0;
	Request *request;
	
	{ // Request line
		string reqline;
		vector<string> tokens;

		getline(stream, reqline);
		if (reqline.length() > 0 && reqline[reqline.length() - 1] == '\r') {
			cr = 1;
			reqline = reqline.substr(0, reqline.length() - 1);
		}
		tokens = tokenize(reqline);
		if (tokens.size() != 3)
			return (new Request(Request::UNKNOWN, "", ""));
		request = new Request(stringToMethod(tokens[0]), tokens[1], tokens[2]);
	}
	{ // Headers
		string header;
		pair<string, string> pair;

		while (getline(stream, header)) {
			if (header.length() == (size_t)cr)
				break ;
			if ((header[header.length() - 1] == '\r') != cr) {
				request->headerCount = -1;
				return (request);
			}
			if (cr)
				header = header.substr(0, header.length() - 1);
			pair = parseHeader(header);
			if (pair.first.empty() || !request->getHeader(pair.first).empty()) {
				request->headerCount = -1;
				return (request);
			}
			request->setHeader(pair.first, pair.second);
		}
	}
	{ // Body
		string line;
		string body;

		while (getline(stream, line)) {
			if (stream.peek() != EOF && (line[line.length() - 1] == '\r') != cr) {
				body = "";
				break;
			}
			body.append(line);
			if (stream.peek() != EOF) {
				body += "\n";
			}
		}
		request->setBody(body);
	}
	return (request);
}

void handleConnection(int fd) {
	string data;
	char buffer[1024];
	Request *request;
	Response *response;

	{ // Read from fd into a string object
		ssize_t readSize;
		while ((readSize = read(fd, buffer, 1024)) > 0)
			data.append(buffer, readSize);
	}
	{ // Parse request
		stringstream stream(data);

		request = parseRequest(data);
		// cout << *request << endl;
	}
	{ // Process request
		processRequest(*request);
	}
}