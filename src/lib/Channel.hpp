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
		const std::string			name;
		std::string					password;
		std::map<int, Client>		users;
		std::vector<int>			operators;
		std::vector<std::string>	messages;
		std::map<char, bool>		mode;
		class InvalidChannelName : public std::exception {
			public:
				const char *what();
		};
	public:
		Channel(const std::string &name, const std::string& password, std::map<char, bool> mode);
		std::map<int, Client>&	clients_in_channel();
		void					add_client_to_channel(int fd, const Client& client);
		void					remove_client_from_channel(int fd);
		void					echo_message_to_channel(const std::string& message);
		void					promote_user_to_operator(int fd);
		void					demote_user_from_operator(int fd);
};

#endif // CHANNEL_HPP
