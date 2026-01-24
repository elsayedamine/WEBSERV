#include <Parse.hpp>

Parse::Parse(Request &req) : state(STATE_METHOD), status(STATE_CURRENT), request(req) {}

void Parse::operator()(std::string data) {
	(void)data;
	while (1) {
		if (status)
			state++;
	}
}

void Parse::parseMethod() {

}

void Parse::parseTarget() {

}

void Parse::parseVersion() {

}

void Parse::parseHeaders() {

}

void Parse::parseBody() {

}
