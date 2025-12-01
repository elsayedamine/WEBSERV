#include <main.hpp>
#include <Request.hpp>
#include <Server.hpp>

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
	pair<string, string> pair;
	size_t mid;

	mid = header.find(':');
	pair.first = strtrim(header.substr(0, mid));
	pair.second = strtrim(header.substr(mid + 1));
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
		if (tokens.size() != 3 || reqline[reqline.length() - 1] == ' ')
			return (new Request("", "", ""));
		request = new Request(tokens[0], tokens[1], tokens[2]);
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
			if (pair.first.empty()) {
				request->headerCount = -1;
				return (request);
			}
			request->setHeader(pair.first, pair.second);
		}
	}
	{ // Body
		string body;
		char ch;

		while (stream.get(ch))
			body.push_back(ch);
		request->setBody(body);
	}
	return (request);
}
