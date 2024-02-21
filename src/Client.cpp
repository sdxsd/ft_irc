#include "lib/Client.hpp"

Client::Client(int client_sockfd): client_sockfd(client_sockfd) {
	;
}

int Client::get_socket() const {
	return (client_sockfd);
}
