#include <Parse.hpp>

Parse::Parse() : state(STATE_METHOD), status(PARSE_CURRENT) {}

void Parse::operator()(std::string data) {
	static stateHandler handlers[] = {
		&Parse::parseMethod,
		&Parse::parseTarget,
		&Parse::parseVersion,
		&Parse::parseHeaders,
		&Parse::parseBody
	};

	current += data;
	while (status != PARSE_OVER && state < 5) {
		(this->*handlers[state])();
		if (status == PARSE_FAIL)
			return;
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
	std::cout << request.getMethod() << std::endl;
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
	std::cout << request.getTarget() << std::endl;
}

void Parse::parseVersion() {
	std::string method = request.getVersion();
	size_t i = 0;

	if (current[i] == ' ') {
		status = PARSE_FAIL;
		return;
	}
	while (i < current.size()) {
		if (!current.compare(i, 2, "\r\n")) {
			i += 2;
			status = PARSE_SUCCESS;
			break;
		}
		method += current[i];
		i++;
	}
	request.setVersion(method);
	current = current.substr(i);
	std::cout << request.getVersion() << std::endl;
}

void Parse::parseHeaders() {

}

void Parse::parseBody() {

}
