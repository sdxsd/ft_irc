#include "lib/Channel.hpp"
#include <iostream>
#include <algorithm>
#include <stdexcept>

Channel::Channel(const std::string &name, const std::string& password, std::map<char, bool> mode):
name(name), password(password), mode(mode) {
	if (name[0] != '#')
		throw InvalidChannelName();
	for (const char& c : "\7 ") // Check for illegal characters within name.
		if (name.find(c) != std::string::npos)
			throw InvalidChannelName();
}

std::map<int, Client>& Channel::clients_in_channel() {
	return (clients);
}

const std::vector<int>& Channel::operators_in_channel() {
	return (operators);
}

bool Channel::is_client_in_channel(int client_sockfd) {
	return ((clients.find(client_sockfd) != clients.end()));
}

void Channel::add_client_to_channel(const Client& client) {
	if (!is_client_in_channel(client.get_socket())) {
		clients.insert({client.get_socket(), client});
		std::cout << "Added client: " << client.get_nickname() << " To: " << name << std::endl;
	}
	// else
	// 	throw std::runtime_error(ERR_); // TODO: Add error for when client attempts to join channel they are in.
}

void Channel::remove_client_from_channel(const Client& client) {
	if (is_client_in_channel(client.get_socket()))
		clients.erase(client.get_socket());
	// else
	// 	; // TODO: Throw error.
}

void Channel::echo_message_to_channel(const std::string& msg) {
	for (auto& u : clients)
		u.second.append_to_messages(msg);
}

void Channel::promote_user_to_operator(int fd) {
	if (std::find(operators.begin(), operators.end(), fd) == operators.end())
		operators.push_back(fd);
}

void Channel::demote_user_from_operator(int fd) {
	auto oper = std::find(operators.begin(), operators.end(), fd);
	if (oper != operators.end())
		operators.erase(oper);
}
