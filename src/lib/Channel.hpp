#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>
#include <vector>
#include "Client.hpp"

#define CHANNEL_NAME_LENGTH_MAX	200;
#define MESSAGE_LENGTH_MAX 		512;

class Channel {
	private:
		const std::string				channel_name;
		std::string						channel_password;
		std::map<unsigned int, Client>	users;
		std::vector<unsigned int>		operators;
		std::vector<std::string>		messages;
		std::map<char, bool>			mode;
	public:
};

#endif // CHANNEL_HPP
