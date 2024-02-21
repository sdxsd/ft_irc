#include "lib/Client.hpp"

Client::Client(int client_sockfd): client_sock(client_sock) {
	;
}

int Client::get_socket() const {
	return (client_sockfd);
}
