#include "lib/Server.hpp"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

Server::Server(uint16_t port, const std::string &password): port(port), password(password)  {
	sockaddr_in	socket_address; // sockaddr_in represents an Internet Protocol address.

	memset(&socket_address, 0, sizeof(sockaddr_in)); // No random data.
	socket_address.sin_family = AF_INET; // Specifies that the socket communicates with IPv4 addresses.
	socket_address.sin_port = htons(port); // htons() converts port to network byte order.
	socket_address.sin_addr.s_addr = INADDR_ANY; // Accept connections from any address.

	server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock == -1) {
		std::cerr << "Error initialising socket" << std::endl;
		exit(1); // TODO: Eventually remove.
	}
	if (bind(server_sock, (sockaddr*)&socket_address, sizeof(socket_address)) == -1) {
		std::cerr << "Error during binding of socket" << std::endl;
		close(server_sock);
		exit(1); // TODO: Eventually remove.
	}
}

int Server::accept_new_clients() {
	sockaddr_in	client;
	socklen_t	client_address_length = sizeof(client);
	return(accept(server_sock, (sockaddr*)&client, &client_address_length));
}

void Server::run(void) {
	unsigned int	client_count = 0;

	if (listen(server_sock, 1024) == -1) { // TODO: Change 1024 to some concrete max client variable.
		std::cerr << "Error setting socket to listen for connections." << std::endl;
		close(server_sock);
		exit(1); // TODO: Eventually remove.
	}
	std::cout << "Server listening on port: " << port << std::endl;
	while (true) { // Main logic (for now)
		int	new_client = accept_new_clients();
		if (new_client != -1) {
			client_count++;
			std::cout << "Client connected!" << std::endl;
			clients.insert(std::make_pair(client_count, Client(new_client)));
		}
	}
}
