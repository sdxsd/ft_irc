#include "lib/Client.hpp"
#include <sys/socket.h>
#include <iostream>

Client::Client(int client_sockfd): client_sockfd(client_sockfd) {
	;
}

void Client::send_message() {
	if (!messages.empty()) {
		const std::string& message = messages.top();
		std::cout << "Sending: " << message;
		if (send(client_sockfd, message.c_str(), message.length(), 0) != -1) {
			messages.pop();
		}
	}
}

int Client::get_socket() const {
	return (client_sockfd);
}
