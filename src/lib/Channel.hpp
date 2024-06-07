#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>
#include <vector>
#include "Client.hpp"

#define CHANNEL_NAME_LENGTH_MAX	200;
#define MESSAGE_LENGTH_MAX 		512;

class Client;

class Channel {
	private:
		const std::string			name;
		std::string					password;
		std::map<int, Client*>		clients;
		std::vector<int>			operators;
		std::vector<std::string>	messages;
		std::map<char, bool>		mode;
	public:
		Channel(const std::string &name, const std::string& password, std::map<char, bool> mode);
		bool							is_client_in_channel(int client_sockfd);
		std::map<int, Client*>&			clients_in_channel();
		const std::vector<int>&			operators_in_channel();
		void							add_client_to_channel(Client& client);
		void							remove_client_from_channel(const Client& client);
		void							echo_message_to_channel(int sender_fd, const std::string& message);
		bool							is_user_operator(int fd);
		void							promote_user_to_operator(int fd);
		void							demote_user_from_operator(int fd);
};

#endif // CHANNEL_HPP
