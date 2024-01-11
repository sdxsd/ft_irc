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
		const std::string				_channelName;
		std::string						_channelPassword;
		std::map<unsigned int, Client>	_users;
		std::vector<unsigned int>		_operators;
		std::vector<std::string>		_messages;
		std::map<char, bool>			_mode;
	public:

};

#endif // CHANNEL_HPP
