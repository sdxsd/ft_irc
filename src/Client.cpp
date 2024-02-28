#include "lib/Client.hpp"
#include <sys/socket.h>

Client::Client(int client_sockfd): client_sockfd(client_sockfd) {
	;
}

void Client::send_message() {
	if (messages.size() > 0) {
		const std::string& message = messages.top();
		if (send(client_sockfd, message.c_str(), message.length(), 0) != -1) {
			messages.pop();
		}
	}
}

int Client::get_socket() const {
	return (client_sockfd);
}
