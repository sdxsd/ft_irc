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
		std::map<int, Client*>		clients;
		std::vector<int>			operators;
		std::vector<std::string>	messages;
		// === MODE DATA === //
		// Pairing of the state of the mode and the data it is associated with.
		std::pair<bool, std::vector<int>>			invite_only; // NOTE: Vector contains invite list.
		std::pair<bool, std::string> 				password_protected;
		std::pair<bool, std::string> 				topic;
		std::pair<bool, int> 						user_limit;

	public:
		Channel(const std::string &name);
		bool						is_client_in_channel(int client_sockfd);
		std::map<int, Client*>&		clients_in_channel();
		const std::vector<int>&		operators_in_channel();
		void						add_client_to_channel(Client& client);
		void						remove_client_from_channel(const Client& client);
		void						echo_message_to_channel(const std::string& message);
		void 						echo_privmsg_to_channel(int sender_fd, const std::string& msg);
		bool						is_user_operator(int fd);
		void						promote_user_to_operator(int fd);
		void						demote_user_from_operator(int fd);
		void						set_topic(const std::string& text);
		void						disable_key();
		void						enable_key(const std::string& key);
		bool						is_key_required();
		int							get_user_limit();
		void						set_user_limit(int user);
		void						remove_user_limit();
		bool						user_limit_set();
		bool 						validate_key(const std::string& key);
		std::string					get_mode() const;
		const std::string&			get_topic() const;
		bool						channel_has_topic();
		void 						set_channel_limit(const bool state);
		void 						add_invite_user(const int user);
		bool 						user_invited(int user);
		void 						set_channel_topic_state(const bool state);
		bool 						is_topic_protected(void);
		bool						is_invite_only(void);
		bool 						in_invite_list(const int user);
};

#endif // CHANNEL_HPP
