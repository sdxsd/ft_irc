#include "lib/Channel.hpp"
#include <iostream>
#include <algorithm>

Channel::Channel(const std::string &name, const std::string& password, std::map<char, bool> mode):
password(password), mode(mode) {
	if (name[0] != '#')
		throw InvalidChannelName();
	for (const char& c : "\7 ") // Check for illegal characters within name.
		if (name.find(c) != std::string::npos)
			throw InvalidChannelName();
}

std::map<int, Client>& Channel::clients_in_channel() {
	return (users);
}

const char *Channel::InvalidChannelName::what() {
	return ("Invalid channel name.");
}

void Channel::add_client_to_channel(int fd, const Client& client) {
	if (users.find(fd) == users.end())
		users.insert(std::make_pair(fd, client));
	else
		std::cerr << "Error when attempting to add user " << fd << "to channel " << name << " USER ALREADY IN CHANNEL" << std::endl;
}

void Channel::remove_client_from_channel(int fd) {
	if (users.erase(fd) == 0)
		std::cerr <<  "Error when attempting to remove user " << fd << "from channel " << name << " USER NOT IN CHANNEL" << std::endl;
	else
		std::cout << "User " << fd << "removed from channel " << name << std::endl;
}

void Channel::echo_message_to_channel(const std::string& msg) {
	for (auto& u : users)
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
