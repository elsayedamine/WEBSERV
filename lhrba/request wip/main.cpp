#include "main.hpp"

int main() {
	int fd[2];
	if (pipe(fd) == -1) {
		perror("pipe");
		return 1;
	}

	const char* http_request =
		"POST /users HTTP/1.1\r\n"
		"Host: example.com\r\n"
		"Content-Type: application/json\r\n"
		"Content-Length: 156\r\n"
		"\r\n"
		"{\r\n"
		"\t\"user\": {\r\n"
		"\t\t\"name\": \"Alice Johnson\",\r\n"
		"\t\t\"email\": \"alice.johnson@example.com\",\r\n"
		"\t\t\"age\": 28,\r\n"
		"\t\t\"preferences\": {\r\n"
		"\t\t\t\"theme\": \"dark\",\r\n"
		"\t\t\t\"notifications\": true\r\n"
		"\t\t}\r\n"
		"\t}\r\n"
		"}";

	// --- write to the "socket" ---
	ssize_t written = write(fd[1], http_request, strlen(http_request));
	if (written == -1) {
		perror("write");
		return 1;
	}
	close(fd[1]); // writer done, simulate end of transmission

	// parseRequest(fd[0]);
	handleConnection(fd[0]);
	// close(fd[0]);
	return 0;
}