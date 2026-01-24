#include <Parse.hpp>

Parse::Parse() : state(STATE_METHOD), status(PARSE_CURRENT) {}

void Parse::operator()(std::string data) {
	static stateHandler handlers[] = {
		&Parse::parseMethod,
		&Parse::parseTarget,
		&Parse::parseVersion,
		// &Parse::parseHeaders,
		// &Parse::parseBody
	};

	current += data;
	while (status != PARSE_OVER) {
		(this->*handlers[state])();		
		if (status == PARSE_SUCCESS)
			state++;
	}
}

void Parse::parseMethod() {
	std::string method = request.getMethod();
	int i = 0;

	if (current[i] == ' ') {
		status = PARSE_FAIL;
		return;
	}
	while (i < current.size()) {
		if (current[i] == ' ') {
			i++;
			status = PARSE_SUCCESS;
		}
		method += current[i];
		i++;
	}
	request.setMethod(method);
	current = current.substr(i);
}

void Parse::parseTarget() {
	std::string method = request.getTarget();
	int i = 0;

	if (current[i] == ' ') {
		status = PARSE_FAIL;
		return;
	}
	while (i < current.size()) {
		if (current[i] == ' ') {
			i++;
			status = PARSE_SUCCESS;
		}
		method += current[i];
		i++;
	}
	request.setTarget(method);
	current = current.substr(i);
}

void Parse::parseVersion() {
	std::string method = request.getVersion();
	int i = 0;

	if (current[i] == ' ') {
		status = PARSE_FAIL;
		return;
	}
	while (i < current.size()) {
		if (!current.compare(i, 2, "\r\n")) {
			i += 2;
			status = PARSE_SUCCESS;
		}
		method += current[i];
		i++;
	}
	request.setVersion(method);
	current = current.substr(i);
}

void Parse::parseHeaders() {

}

void Parse::parseBody() {

}
