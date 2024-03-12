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
	poll_sockfds.push_back({server_sockfd, POLLIN, 0});
}

void Server::accept_new_client() {
	sockaddr_in	client_addr;
	int			sockfd;
	socklen_t	client_address_length = sizeof(client_addr);

	sockfd = accept(server_sockfd, (sockaddr*)&client_addr, &client_address_length);
	if (sockfd != -1) {
		std::cout << "Client connected!" << std::endl; // TODO: TEMP
		poll_sockfds.push_back({sockfd, POLLIN | POLLOUT, 0});
		clients.insert(std::make_pair(sockfd, Client(sockfd))); // Client ID represented by the file descriptor used to communicate with them.
		// clients.find(sockfd)->second.messages.push("001 keizerrijk :Welcome to the server!\r\n");
	}
}

void Server::send_to_channel(const std::string& channel_name, const std::string &message) {
	Channel& channel = channels.find(channel_name)->second;
	for (auto& c : channel.clients_in_channel())
		c.second.messages.push(message);
}

void Server::run(void) {
	if (listen(server_sockfd, 1024) == -1) { // TODO: Change 1024 to some concrete max client variable.
		std::cerr << "Error setting socket to listen for connections." << std::endl;
		close(server_sockfd);
		exit(1); // TODO: Eventually remove.
	}
	std::cout << "Server listening on port: " << port << std::endl;
	while (true) { // TODO: Main logic (for now)
		int	presult = poll(poll_sockfds.data(), poll_sockfds.size(), -1);
		if (presult != -1 && presult >= 1) {
			for (const pollfd& pfd : poll_sockfds) {
				if (pfd.revents & POLLIN) {
					if (pfd.fd == server_sockfd)
						accept_new_client();
					else {
						recv(pfd.fd, &clients.find(pfd.fd)->second.recv_buffer[0], BUFSIZE, 0);
						clients.find(pfd.fd)->second.recv_buffer->clear();
					}
				}
				else if (pfd.revents & POLLOUT) {
					clients.find(pfd.fd)->second.send_message();
				}
			}
		}
	}
}
