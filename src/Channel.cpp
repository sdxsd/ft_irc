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
