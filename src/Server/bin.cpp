#include <Server.hpp>


void	Server::run() 
{
	int epoll_fd;
	if ((epoll_fd = epoll_create1(0)) < 0)
		throw std::runtime_error("epoll() failed.");
	for (std::size_t i = 0; i < sockets.size(); ++i) {
		struct epoll_event ep;
		ep.events = EPOLLIN;
		ep.data.fd = sockets[i];
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockets[i], &ep) < 0)
        throw std::runtime_error("epoll_ctl failed to add listener.");
	}

	struct epoll_event events[128];
	std::cout << "Server is running..." << std::endl;

	while (true)
	{
		int nfds = epoll_wait(epoll_fd, events, 128, -1);
		if (nfds == -1) continue ;
		for (int i = 0; i < nfds; ++i)
		{
			int curr = events[i].data.fd;
			if (is_listener(curr)) {
				struct sockaddr_in client_addr;
				socklen_t client_len = sizeof(client_addr);
				int client_socket = accept(curr, (struct sockaddr *)&client_addr, &client_len);
				if (client_socket < 0) {
					std::cerr << "Accept failed" << std::endl;
					continue;
				}

				fcntl(client_socket, F_SETFL, O_NONBLOCK);

				struct epoll_event client_event;
				client_event.events = EPOLLIN;
				client_event.data.fd = client_socket;

				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &client_event) < 0) {
					std::cerr << "Failed to add client to epoll" << std::endl;
					close(client_socket);
				}
				else
					std::cout << "New connection on FD: " << client_socket << std::endl;
			}
			else {
				char buffer[1024];
				// Try to read data
				int bytes_read = recv(curr, buffer, sizeof(buffer) - 1, 0);

				if (bytes_read <= 0) {
					// Connection closed or Error
					if (bytes_read == 0)
						std::cout << "Client disconnected: " << curr << std::endl;
					else
						std::cerr << "Recv error on: " << curr << std::endl;

					// 1. Remove from Epoll (Kernel automatically removes closed FDs, but good practice to be explicit)
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, curr, NULL);

					// 2. Close the socket
					close(curr);
				} 
				else {
					// SUCCESS: We have data!
					buffer[bytes_read] = '\0';
					std::cout << "Received " << bytes_read << " bytes from FD " << curr << std::endl;

					// TODO: Pass 'buffer' to your HTTP Request Parser
					// handle_request(curr, buffer);
				}

			}
		}
		
	}

	// handleConnection(1, (config.getServers())[0]);
}
