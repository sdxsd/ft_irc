#include "lib/Server.hpp"
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

Server::Server(uint16_t port, const std::string &password): port(port), password(password)  {
	sockaddr_in	socket_address; // sockaddr_in represents an Internet Protocol address.

	memset(&socket_address, 0, sizeof(sockaddr_in)); // No random data.
	socket_address.sin_family = AF_INET; // Specifies that the socket communicates with IPv4 addresses.
	socket_address.sin_port = htons(port); // htons() converts port to network byte order.
	socket_address.sin_addr.s_addr = INADDR_ANY; // Accept connections from any address.
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sockfd == -1) {
		std::cerr << "Error initialising socket" << std::endl;
		exit(1); // TODO: Eventually remove.
	}
	if (bind(server_sockfd, (sockaddr*)&socket_address, sizeof(socket_address)) == -1) {
		std::cerr << "Error during binding of socket" << std::endl;
		close(server_sockfd);
		exit(1); // TODO: Eventually remove.
	}
	poll_sockfds.push_back({server_sockfd, POLLIN | POLLOUT, 0});
}

void Server::accept_new_client() {
	sockaddr_in	client;
	int			sockfd;
	socklen_t	client_address_length = sizeof(client);

	sockfd = accept(server_sockfd, (sockaddr*)&client, &client_address_length);
	if (sockfd != -1) {
		std::cout << "Client connected!" << std::endl; // TODO: TEMP
		poll_sockfds.push_back({sockfd, POLLIN | POLLOUT, 0});
		clients.insert(std::make_pair(sockfd, Client(sockfd))); // Client ID represented by the file descriptor used to communicate with them.
	}
}

void Server::run(void) {
	const char buf[] = "ZoepZoep\n";
	char ibuf[1024];

	if (listen(server_sockfd, 1024) == -1) { // TODO: Change 1024 to some concrete max client variable.
		std::cerr << "Error setting socket to listen for connections." << std::endl;
		close(server_sockfd);
		exit(1); // TODO: Eventually remove.
	}
	std::cout << "Server listening on port: " << port << std::endl;
	while (true) { // TODO: Main logic (for now)
		if (poll(poll_sockfds.data(), poll_sockfds.size(), 1024) != -1) {
			for (const pollfd& pfd : poll_sockfds) {
				if (pfd.fd == server_sockfd && pfd.revents & POLLIN)
					accept_new_client();
				else if (pfd.revents & POLLOUT) {
					std::cout << "POLLOUT" << std::endl;
					send(pfd.fd, (void *)&buf, strlen(buf), 0);
				}
				else if (pfd.revents & POLLIN) {
					std::cout << "POLLIN" << std::endl;
					recv(pfd.fd, (void *)&ibuf, 1024, 0);
					std::cout << ibuf << std::endl;
				}
			}
		}
		// for (const auto& [id, client] : clients) {
		// 	send(client.get_socket(), (void *)&buf, strlen(buf), 0);
		// }
	}
}
