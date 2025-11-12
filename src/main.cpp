#include <main.hpp>

int main() {
	int fd[2];
	const char* http_request =
		"POST /users HTTP/1.1\r\n"
		"Host: example.com\r\n"
		"Content-Type: application/json\r\n"
		"Content-Length: 175\r\n"
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
		
	pipe(fd);
	write(fd[1], http_request, strlen(http_request));
	close(fd[1]);
	handleConnection(fd[0]);
	return 0;
}