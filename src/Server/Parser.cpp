#include <Parser.hpp>
#include <Utils.hpp>

Parser::Parser() : state(STATE_METHOD), status(PARSE_CURRENT) {}

const Request &Parser::getRequest() const { return request; }

int Parser::getStatus() const { return status; }

void Parser::setStatus(::status s) { status = s; }

void Parser::operator()(std::string data) {
	static stateHandler handlers[] = {
		&Parser::parseMethod,
		&Parser::parseTarget,
		&Parser::parseQuery,
		&Parser::parseVersion,
		&Parser::parseHeader,
		&Parser::parseBody
	};

	current += data;
	while (status != PARSE_OVER && status != PARSE_FAIL) {
		(this->*handlers[state])();
		if (status == PARSE_PENDING)
			return;
		if (status == PARSE_SUCCESS) {
			status = PARSE_CURRENT;
			state++;
		}
	}
	if (status == PARSE_OVER || status == PARSE_FAIL)
		request.setReady(true);
}

void Parser::parseMethod() {
	std::string method = request.getMethod();
	size_t i = 0;

	if (current.empty() || current[i] == ' ') {
		status = PARSE_FAIL;
		return;
	}
	while (i < current.size()) {
		if (current[i] == ' ') {
			i++;
			status = PARSE_SUCCESS;
			break;
		}
		method += current[i];
		i++;
	}
	if (status == PARSE_SUCCESS && !validateMethod(method))
		status = PARSE_FAIL;
	current = current.substr(i);
	request.setMethod(method);
}

void Parser::parseTarget() {
	std::string target = request.getTarget();
	size_t i = 0;

	if (current[i] == ' ') {
		status = PARSE_FAIL;
		return;
	}
	while (i < current.size()) {
		if (current[i] == ' ' || current[i] == '?') {
			i += current[i] == '?';
			status = PARSE_SUCCESS;
			break;
		}
		target += current[i];
		i++;
	}
	if (status == PARSE_SUCCESS && !validateTarget(target))
		status = PARSE_FAIL;
	request.setTarget(target);
	current = current.substr(i);
}

void Parser::parseQuery() {
	std::string query = request.getQuery();
	size_t i = 0;

	while (i < current.size()) {
		if (current[i] == ' ') {
			i++;
			status = PARSE_SUCCESS;
			break;
		}
		query += current[i];
		i++;
	}
	if (status == PARSE_SUCCESS && !validateQuery(query))
		status = PARSE_FAIL;
	request.setQuery(query);
	current = current.substr(i);
}

void Parser::parseVersion() {
	std::string version = request.getVersion();
	size_t i = 0;

	if (current[i] == ' ') {
		status = PARSE_FAIL;
		return;
	}
	while (i < current.size()) {
		if (current[i] == '\n') {
			cr = current[i - 1] == '\r' ? 1 : 0;
			i += 1;
			status = PARSE_SUCCESS;
			break;
		}
		version += current[i];
		i++;
	}
	version = version.substr(0, version.size() - cr);
	if (status == PARSE_SUCCESS && !validateVersion(version))
		status = PARSE_FAIL;
	request.setVersion(version);
	current = current.substr(i);
}

void Parser::parseHeader() {
	size_t i = 0;
	std::pair<std::string, std::string> header;

	if (current[i] == ' ') {
		status = PARSE_FAIL;
		return;
	}
	if (current.find("\n") == std::string::npos) {
		status = PARSE_PENDING;
		return;
	}
	status = PARSE_CURRENT;
	i += current[i] == '\r';
	if (checkNL(i)) {
		current = current.substr(i);
		status = PARSE_SUCCESS;
		return;
	}

	while (i < current.size()) {
		if (checkNL(i) || current[i] == ' ') {
			status = PARSE_FAIL;
			return;
		}
		if (current[i] == ':')
			break;
		header.first += current[i];
		i++;
	}
	while (++i < current.size()) {
		if (checkNL(i))
			break;
		header.second += current[i];
	}
	header.second = strtrim(header.second.substr(0, header.second.size() - cr));
	if (!validateHeader(header, request.getHeaders()))
		status = PARSE_FAIL;
	request.setHeader(header.first, header.second);
	current = current.substr(i);
}

void Parser::parseBody() {
	std::string body = request.getBody();
	std::string content_length = request.getHeader("Content-Length");
	size_t length = stringToInt(content_length);

	if (length <= body.size()) {
		status = PARSE_OVER;
		return;
	}
	size_t remaining = length - body.size();
	size_t chunk = min(current.size(), remaining);

	body.append(current, 0, chunk);
	request.setBody(body);
	current = current.substr(chunk);
	if (body.size() >= length)
		status = PARSE_OVER;
	else
		status = PARSE_PENDING;
}

int validateMethod(const std::string meth) {
	static std::string inv = "()<>@,;:\"/[]?={}";

	for (size_t i = 0; i < inv.size(); i++) {
		if (meth.find(inv[i]) != std::string::npos)
			return (0);
	}
	return (1);
}

int validateTarget(const std::string tar) {
	if (tar[0] == '/')
		return (1);
	return (0);
}

int validateQuery(const std::string que) {
	char c = '&';

	for (size_t i = 0; i < que.size(); i++) {
		if (c == '&' && que[i] == '=') {
			c = '=';
			continue;
		}
		if (c == '=' && que[i] == '&') {
			c = '&';
			continue;
		}
		if (c == '=' && que[i] == '=')
			return (0);
	}
	return (1);
}

int validateVersion(const std::string ver) {
	if (ver.length() != 8 || ver.compare(0, 5, "HTTP/"))
		return (0);
	return (1);
}

int validateHeader(const std::pair<std::string, std::string> head, const std::multimap<std::string, std::string> map) {
	if (head.first.empty())
		return (0);
	for (int i = 0; (size_t)i < head.first.length(); i++) {
		if (!isalnum(head.first[i]) && head.first[i] != '-')
			return (0);
	}
	for (int i = 0; (size_t)i < head.second.length(); i++) {
		if (head.second[i] < 32 || head.second[i] > 126)
			return (0);
	}
	return (map.count(head.first) == 0);
}

int validateBody(const std::string str) {
	(void)str;
	return (true);
}

int Parser::checkNL(size_t &i) {
	if (current[i] != '\n')
		return (0);
	i++;
	if (cr)
		return (current[i - 2] == '\r');
	return (current[i - 2] != '\r');
}
