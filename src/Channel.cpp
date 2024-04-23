#include "lib/Channel.hpp"

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
}

void Channel::remove_client_from_channel_by_fd(int fd) {
	users.erase(fd);
}
