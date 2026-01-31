#include <Parse.hpp>

Parse::Parse() : state(STATE_METHOD), status(PARSE_CURRENT) {}

void Parse::operator()(std::string data) {
	static stateHandler handlers[] = {
		&Parse::parseMethod,
		&Parse::parseTarget,
		&Parse::parseVersion,
		&Parse::parseHeader,
		&Parse::parseBody
	};

	current += data;
	while (status != PARSE_OVER && state < 5) {
		(this->*handlers[state])();
		if (status == PARSE_FAIL) {
			std::cout << "parse fail" << std::endl;
			return;
		}
		if (status == PARSE_SUCCESS) {
			status = PARSE_CURRENT;
			state++;
		}
	}
}

void Parse::parseMethod() {
	std::string method = request.getMethod();
	size_t i = 0;

	if (current[i] == ' ') {
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
	request.setMethod(method);
	current = current.substr(i);
}

void Parse::parseTarget() {
	std::string target = request.getTarget();
	size_t i = 0;

	if (current[i] == ' ') {
		status = PARSE_FAIL;
		return;
	}
	while (i < current.size()) {
		if (current[i] == ' ') {
			i++;
			status = PARSE_SUCCESS;
			break;
		}
		target += current[i];
		i++;
	}
	request.setTarget(target);
	current = current.substr(i);
}

void Parse::parseVersion() {
	std::string version = request.getVersion();
	size_t i = 0;

	if (current[i] == ' ') {
		status = PARSE_FAIL;
		return;
	}
	while (i < current.size()) {
		if (current[i] == '\n') {
			cr = current[i - 1] == '\r' ? 1 : 0;
			i += cr;
			status = PARSE_SUCCESS;
			break;
		}
		version += current[i];
		i++;
	}
	request.setVersion(version);
	current = current.substr(i);
}

void Parse::parseHeader() {
	size_t i = 0;

	if (current[i] == ' ') {
		status = PARSE_FAIL;
		return;
	}
	if (checkNL(i)) {
		status = PARSE_SUCCESS;
		return;
	}
	if (current.find("\n") == std::string::npos)
		return;

	std::pair<std::string, std::string> header;

	while (i < current.size()) {
		if (checkNL(i)) {
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
	}
	header.second += current[i];
	request.setHeader(header.first, header.second);
	current = current.substr(i);
}

void Parse::parseBody() {
	std::string body = request.getBody();
	size_t i = 0;

	if (checkNL(i)) {
		status = PARSE_OVER;
		return;
	}
	while (i < current.size()) {
		body += current[i];
		i++;
	}
	request.setBody(body);
	current = current.substr(i);
}

int Parse::checkNL(size_t &i) {
	if (current[i] != '\n')
		return (0);
	i++;
	if (cr)
		return (current[i - 2] == '\r');
	return (current[i - 2] != '\r');
}
