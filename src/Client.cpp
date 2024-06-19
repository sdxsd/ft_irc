#include "lib/Client.hpp"
#include <stdexcept>
#include <sys/socket.h>
#include "lib/Replies.hpp"
#include "lib/utils.hpp"
#include <vector>
#include <string>
#include <iostream>

Client::Client(int sockfd): client_sockfd(sockfd) {
	nickname = "*"; // * Is often used as a placeholder.
	disconnection_reason = "";
	registered = false;
	password_valid = false;
	to_disconnect = false;
}

Client::~Client() {
	;
}

void Client::mark_for_disconnection(const std::string& reason) {
	disconnection_reason = reason;
	to_disconnect = true;
}

const std::string&	Client::get_servername() const {
	return (servername);
}

std::string Client::get_hostmask() const {
	return (nickname + "!" + username + "@" + hostname);
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
		const std::string message = messages.front();
		if (send(client_sockfd, message.c_str(), message.length(), 0) != -1) {
			std::cout << "To " << nickname << ": "  << message << std::endl;
			messages.pop();
		}
	}
}

unsigned long Client::messages_queue_size() const {
	return (messages.size());
}

const std::string& Client::get_disconnect_reason() const {
	return (disconnection_reason);
}

bool Client::awaiting_disconnect() const {
	return (to_disconnect);
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

bool Client::has_valid_password() const {
	return (password_valid);
}

void Client::set_password_validity(bool state) {
	password_valid = state;
}

bool Client::is_valid_client() const {
	if (password_valid == true && registered == true)
		return (true);
	return (false);
}

void Client::register_client(std::vector<std::string> &args) {
	if (registered == true)
		throw std::runtime_error(ERR_ALREADYREGISTERED(nickname));
	if (args.size() < 5)
		throw std::runtime_error(ERR_NEEDMOREPARAMS(nickname, args[0]));
	if (args[1].size() > 9)
		username = args[1].substr(0, 9);
	else
		username = args[1];
	hostname = args[2];
	servername = args[3];
	realname = "";
	for (unsigned long i = 4; i < args.size(); i++)
		realname += (args[i] + " ");
	realname = trimWhitespace(realname);
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
