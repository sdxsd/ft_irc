#include "lib/Client.hpp"
#include <stdexcept>
#include <sys/socket.h>
#include "lib/Replies.hpp"
#include <vector>
#include <string>
#include <iostream>

Client::Client(int sockfd): client_sockfd(sockfd) {
	nickname = "*"; // * Is often used as a placeholder.
	registered = false;
}

Client::~Client() {
	;
}

std::string Client::get_recv_buffer() {
	return (recv_buffer);
}

void Client::append_to_recv_buffer(const std::string& buf) {
	recv_buffer += buf;
}

void Client::clear_recv_buffer() {
	recv_buffer.clear();
}

void Client::send_message() {
	if (!messages.empty()) {
		const std::string message = messages.top();
		if (send(client_sockfd, message.c_str(), message.length(), 0) != -1) {
			std::cout << "To client: " << message << std::endl;
			messages.pop();
		}
	}
}

void Client::replyPing(Client &client) {
	std::cout << "hostname for ping: [" << client.get_hostname() << std::endl;
	std::string msg = "PONG " + client.get_hostname() + "\r\n";
	client.append_to_messages(msg);
}

void Client::set_nickname(std::string nick) {
	if (!nickname.empty() || nickname == "<client>")
		old_nickname = nickname;
	nickname = nick;
}

bool Client::is_registered() const {
	return (registered);
}

void Client::register_client(std::vector<std::string> &args) {
	if (args.size() != 5)
		throw std::runtime_error(ERR_NEEDMOREPARAMS(nickname, args[0]));
	if (registered == true)
		throw std::runtime_error(ERR_ALREADYREGISTERED(nickname));
	username = args[1];
	hostname = args[2];
	servername = args[3];
	realname = args[4];
	registered = true;
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

const std::string& Client::get_old_nickname() const {
	return (old_nickname);
}

const std::string& Client::get_hostname() const {
	return (hostname);
}

const std::string& Client::get_realname() const {
	return (realname);
}
