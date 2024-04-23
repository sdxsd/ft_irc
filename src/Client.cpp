#include "lib/Client.hpp"
#include <sys/socket.h>

Client::Client(int client_sockfd): client_sockfd(client_sockfd) {
	;
}

Client::~Client() {
	;
}

const std::string& Client::get_recv_buffer() const {
	return (recv_buffer);
}

void Client::append_to_recv_buffer(const std::string& buf) {
	recv_buffer += buf;
}

void Client::send_message() {
	if (!messages.empty()) {
		const std::string& message = messages.top();
		if (send(client_sockfd, message.c_str(), message.length(), 0) != -1)
			messages.pop();
	}
}

void Client::append_to_messages(const std::string& msg) {
	messages.push(msg);
}

int Client::get_socket() const {
	return (client_sockfd);
}

const std::string& Client::get_username() const {
	return (username);
}

const std::string& Client::get_nickname() const {
	return (nickname);
}

const std::string& Client::get_hostname() const {
	return (hostname);
}

const std::string& Client::get_realname() const {
	return (realname);
}
