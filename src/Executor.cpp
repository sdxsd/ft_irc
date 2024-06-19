#include "lib/Server.hpp"
#include "lib/Replies.hpp"
#include "lib/utils.hpp"
#include <functional>
#include <stdexcept>
#include <iostream>
#include <utility>

// Since an if/else tree feels wrong to me, and switch statements do not work
// with strings I have created this command_map.
// This may seem slightly cursed, but think of it like this.
// Let's say you have a function called blorb which should be called by the program
// when a user inputs "blorb" as a string. Wouldn't it be convenient to just be
// able to match the string "blorb" to the function with the same name?
// This would be much simpler than the equivalent if statement where one must first
// check if the input is "blorb" and then explicitly call the blorb function.
// What this map does, is explicitly associate command names with their associated functions.
// Hence you can think of these not as anonymous functions, but as functions with a name
// that can be referenced by the program. Rather than the function names being discarded by the
// compiler, they are preserved and usable by the program in mapping input directly to action.
int Server::execute_cmd(std::vector<std::string>& args, Client& client) {
	const std::map<std::string, std::function<int()>> command_map {
		{
			"CAP", [&]() -> int {
				if (args.size() > 2 && args[1] == "LS" && args[2] == "302")
					client.append_to_messages(":localhost CAP * LS :\r\n");
				return (true);
			},
		},

		{
			"NICK", [&]() -> int {
				if (args.size() != 2)
					throw std::runtime_error(ERR_NONICKNAMEGIVEN(client.get_hostname()));
				if (!client.has_valid_password()) {
					client.append_to_messages(ERR_PASSWDMISMATCH(client.get_nickname()));
					client.mark_for_disconnection(":Incorrect Password");
					return (false);
				}
				if (args[1][0] == '#' || args[1][0] == '&' || args[1][0] == ':' || args[1][0] == ' ')
					throw std::runtime_error(ERR_ERRONEUSNICKNAME(client.get_nickname(), args[1]));
				for (auto& c : clients)
					if (c.second.get_nickname() == args[1])
						throw std::runtime_error(ERR_NICKNAMEINUSE(client.get_nickname(), args[1]));
				client.set_nickname(args[1]);
				client.append_to_messages(RPL_NICK(client.get_old_nickname(), client.get_nickname()));
				return (true);
			},
		},

		{
			"PASS", [&]() -> int {
				if (args.size() != 2)
					throw std::runtime_error(ERR_NEEDMOREPARAMS(client.get_nickname(), args[0]));
				if (client.is_registered())
					throw std::runtime_error(ERR_ALREADYREGISTERED(client.get_nickname()));
				std::cout << "Password set: "<< args[1] << std::endl;
				if (args[1] != password) {
					client.append_to_messages(ERR_PASSWDMISMATCH(client.get_nickname()));
					client.mark_for_disconnection(":Incorrect Password");
					return (false);
				}
				else
					client.set_password_validity(true);
				return (true);
			},
		},

		{
			"USER", [&]() -> int {
				if (client.has_valid_password() == false) {
					client.mark_for_disconnection(":Incorrect Password");
					return (false);
				}
				client.register_client(args);
				client.append_to_messages(RPL_WELCOME(client.get_nickname(), client.get_nickname()));
				return (true);
			},
		},

		{
			"TOPIC", [&]() -> int {
				if (args.size() < 2)
					throw std::runtime_error(ERR_NEEDMOREPARAMS(client.get_nickname(), args[0]));
				Channel& channel = find_channel(client.get_nickname(), args[1]);
				if (!(channel.is_client_in_channel(client.get_socket())))
					throw std::runtime_error(ERR_NOTONCHANNEL(client.get_nickname(), args[1]));
				if (channel.is_topic_protected()){
					if(!channel.is_user_operator(client.get_socket()))
						throw std::runtime_error(ERR_CHANOPRIVSNEEDED(client.get_nickname(), args[1]));
				}
				if (args.size() == 3)
					channel.set_topic(args[2]);
				else if (args.size() == 2)
					channel.set_topic("");
				if (channel.channel_has_topic()){
					channel.echo_privmsg_to_channel(client.get_socket(), RPL_TOPIC(client.get_nickname(), args[1], channel.get_topic()));
					client.append_to_messages(RPL_TOPIC(client.get_nickname(), args[1], channel.get_topic()));
				}
				else {
					channel.echo_privmsg_to_channel(client.get_socket(), RPL_NOTOPIC(client.get_nickname(), args[1]));
					client.append_to_messages(RPL_NOTOPIC(client.get_nickname(), args[1]));
				}
				return (true);
			},
		},

		{
			"PING", [&]() -> int {
				client.append_to_messages(RPL_PING(client.get_servername(), args[1]));
				return (true);
			},
		},

		{
			"PART", [&]() -> int {
				if (args.size() < 2)
					throw std::runtime_error(ERR_NEEDMOREPARAMS(client.get_nickname(), args[0]));
				if (args[1][0] != '#')
					throw std::runtime_error(ERR_BADCHANMASK(client.get_nickname(), args[1]));
				Channel& channel = find_channel(client.get_nickname(), args[1]);
				if (!(channel.is_client_in_channel(client.get_socket())))
					throw std::runtime_error(ERR_NOTONCHANNEL(client.get_nickname(), args[1]));
				std::string reason = "";
				if (args.size() > 2) {
					for (unsigned long i = 2; i < args.size(); i++)
						reason += (args[i] + " ");
					reason = trimWhitespace(reason);
					channel.echo_message_to_channel(RPL_PART(client.get_hostmask(), args[1], reason));
				}
				else
					channel.echo_message_to_channel(RPL_PART(client.get_hostmask(), args[1], reason));
				channel.remove_client_from_channel(client);
				if (channel.clients_in_channel().size() < 1)
					channels.erase(args[1]);
				return (true);
			},
		},

		{
			"QUIT", [&]() -> int {
				std::string msg = (args.size() > 1) ? args[1] : "";
				for (auto& channel : channels)
					if (channel.second.is_client_in_channel(client.get_socket()))
						channel.second.echo_message_to_channel(RPL_QUIT(client.get_hostmask(), msg));
				client.mark_for_disconnection("Quit");
				return (false);
			},
		},

		{
			"NAMES", [&]() -> int {
				if (args.size() < 2)
					throw std::runtime_error(ERR_NEEDMOREPARAMS(client.get_nickname(), args[0]));
				Channel& channel = find_channel(client.get_nickname(), args[1]);
				for (auto& c : channel.clients_in_channel()) {
					std::string nick_and_prefix = "";
					if (channel.is_user_operator(c.first) == true) // Is client operator?
						nick_and_prefix = ("@" + c.second->get_nickname());
					else
						nick_and_prefix = client.get_nickname();
					//client.append_to_messages(RPL_NAMREPLY(client.get_nickname(), args[1], nick_and_prefix));
					channel.echo_message_to_channel(RPL_NAMREPLY(client.get_nickname(), args[1], nick_and_prefix));	
				}
				//client.append_to_messages(RPL_ENDOFNAMES(client.get_nickname(), args[1]));
				channel.echo_message_to_channel(RPL_ENDOFNAMES(client.get_nickname(), args[1]));
				return (true);
			}
		},

		{
			"JOIN", [&]() -> int {
				if (args.size() < 2)
					throw std::runtime_error(ERR_NEEDMOREPARAMS(client.get_nickname(), args[0]));
				if (args[1] == ":")
					return (true);
				if (!client.is_valid_client())
					throw std::runtime_error(ERR_NOTREGISTERED(client.get_nickname()));
				auto channel = channels.find(args[1]);
				if (channel != channels.end()) {
					if (channel->second.is_key_required() == true){
						if (args.size() < 3 || channel->second.validate_key(args[2]) == false)
							throw std::runtime_error(ERR_BADCHANNELKEY(client.get_nickname(), args[1]));
					}
					if (channel->second.is_invite_only() && !channel->second.in_invite_list(client.get_socket()))
						throw std::runtime_error(ERR_INVITEONLYCHAN(client.get_nickname(), args[1]));
					channel->second.add_client_to_channel(client);
					std::cout << "Client " << client.get_nickname() << " has been added to channel." << std::endl;
				}
				else {
					auto new_channel = channels.insert({args[1], Channel(args[1])});
					if (new_channel.second == false) // NOTE: This checks if the channel was actually inserted.
						throw std::runtime_error(ERR_UNKNOWNERROR(client.get_nickname(), args[0], std::string("Failed to create channel.")));
					channel = new_channel.first;
					channel->second.add_client_to_channel(client);
					channel->second.promote_user_to_operator(client.get_socket());
					std::cout << "Channel successfully created" << std::endl;
				}
				channel->second.echo_message_to_channel(RPL_JOIN(client.get_hostmask(), args[1]));
				if (channel->second.channel_has_topic())
					client.append_to_messages(RPL_TOPIC(client.get_nickname(), args[1], channel->second.get_topic()));
				std::vector<std::string> name_args{"NAMES", args[1]};
				execute_cmd(name_args, client);
				return (true);
			},
		},

		{
			"PRIVMSG", [&]() -> int { // NOTE: In the interest of a simpler parsing system we have chosen to disallow multiple target messages.
				if (!client.is_valid_client())
					throw std::runtime_error(ERR_NOTREGISTERED(client.get_nickname()));
				const std::string& target = args[1];
				std::string msg = "";
				for (unsigned long i = 2; i < args.size(); i++) {
					msg += (args[i] + " ");
				}
				msg = trimWhitespace(msg);
				if (target[0] == '#') { // Target is channel.
					Channel& channel = find_channel(client.get_nickname(), args[1]);
					channel.echo_privmsg_to_channel(client.get_socket(), RPL_PRIVMSG(client.get_nickname(), target, msg));
				}
				else { // Sending to user.
					Client& receiver = find_user(client.get_nickname(), target);
					receiver.append_to_messages(RPL_PRIVMSG(client.get_nickname(), target, msg));
				}
				return (true);
			},
		},

		{
			"NOTICE", [&]() -> int {
				if (!client.is_valid_client())
					return (false);
				const std::string& target = args[1];
				std::string msg = "";
				for (unsigned long i = 2; i < args.size(); i++) {
					msg += (args[i] + " ");
				}
				msg = trimWhitespace(msg);
				auto channel = channels.find(target);
				if (channel != channels.end())
					channel->second.echo_privmsg_to_channel(client.get_socket(), RPL_PRIVMSG(client.get_nickname(), target, msg));
				else
					return (false);
				return (true);
			},
		},

		{
			"INVITE", [&]() -> int {
				if (args.size() < 3)
					throw std::runtime_error(ERR_NEEDMOREPARAMS(client.get_nickname(), args[0]));
				Channel& channel = find_channel(client.get_nickname(), args[2]);
				if (!channel.is_client_in_channel(client.get_socket()))
					throw std::runtime_error(ERR_USERNOTINCHANNEL(client.get_nickname(), client.get_nickname(), args[2]));
				Client& user = find_user(client.get_nickname(), args[1]);
				if (channel.is_client_in_channel(user.get_socket()))
					throw std::runtime_error(ERR_USERONCHANNEL(client.get_nickname(), args[1], args[2]));
				if (!channel.is_user_operator(client.get_socket()))
					throw std::runtime_error(ERR_CHANOPRIVSNEEDED(client.get_nickname(), args[2]));
				channel.add_invite_user(user.get_socket()); 
				client.append_to_messages(RPL_INVITING(client.get_hostmask(), client.get_nickname(), user.get_nickname(), args[2]));
				user.append_to_messages(RPL_INVITE(client.get_hostmask(), user.get_nickname(), args[2]));
				return (true);
			},
		},

		{
			"WHO", [&]() -> int {
				if (args.size() < 2)
					throw std::runtime_error(ERR_NEEDMOREPARAMS(client.get_nickname(), args[0]));
				std::string& target = args[1];
				if (target[0] != '#') {
					Client& user = find_user(client.get_nickname(), target);
					std::string reply = RPL_WHOREPLY(client.get_nickname(), "*", user.get_username(), user.get_hostname(), user.get_servername(), user.get_nickname(), user.get_realname());
					client.append_to_messages(reply);
				}
				else {
					Channel& channel = find_channel(client.get_nickname(), target);
					for (auto &c : channel.clients_in_channel()) {
						Client& user = *c.second;
						client.append_to_messages(RPL_WHOREPLY(client.get_nickname(), "*", user.get_username(), user.get_hostname(), user.get_servername(), user.get_nickname(), user.get_realname()));
					}
				}
				client.append_to_messages(RPL_ENDOFWHO(client.get_nickname(), target));
				return (true);
			},
		},

		{
			"KICK", [&]() -> int {
				if (args.size() < 3)
					throw std::runtime_error(ERR_NEEDMOREPARAMS(client.get_nickname(), args[0]));
				Channel& channel = find_channel(client.get_nickname(), args[1]);
				if (!channel.is_client_in_channel(client.get_socket()))
					throw std::runtime_error(ERR_NOTONCHANNEL(client.get_nickname(), args[1]));
				Client& user = find_user(client.get_nickname(), args[2]);
				if (!channel.is_client_in_channel(user.get_socket()))
					throw std::runtime_error(ERR_USERNOTINCHANNEL(client.get_nickname(), args[2], args[1]));
				if (!channel.is_user_operator(client.get_socket()))
					throw std::runtime_error(ERR_CHANOPRIVSNEEDED(client.get_nickname(), args[1]));
				std::vector<std::string> forced_part{"PART", args[1], ":forcibly kicked"};
				execute_cmd(forced_part, user); // NOTE: Based or cringe?
				std::string msg = ":Default Reason";
				if (args.size() > 3) {
					msg.clear();
					for (unsigned long i = 3; i < args.size(); i++)
						msg += args[1] + " ";
					msg = trimWhitespace(msg);
				}
				channel.echo_message_to_channel(RPL_KICK(client.get_nickname(), args[1], args[2], msg));
				return (true);
			},
		},

		{
			"MODE", [&]() -> int {
				if (args.size() < 2)
					throw std::runtime_error(ERR_NEEDMOREPARAMS(client.get_nickname(), args[0]));
				if (args[1][0] != '#')
					return (true);
				Channel& channel = find_channel(client.get_nickname(), args[1]);
				if (args.size() < 3) // NOTE: No modestring given, send the client the channels modes.
					client.append_to_messages(RPL_CHANNELMODEIS(client.get_nickname(), args[1], channel.get_mode()));
				else {
					if (args[2] == "b")
						throw std::runtime_error(RPL_ENDOFBANLIST(client.get_nickname(), args[1]));
					if (!channel.is_client_in_channel(client.get_socket()))
						throw std::runtime_error(ERR_USERNOTINCHANNEL(client.get_nickname(), client.get_nickname(), args[1]));
					if (!(channel.is_user_operator(client.get_socket())))
						throw std::runtime_error(ERR_CHANOPRIVSNEEDED(client.get_nickname(), args[1]));
					bool state;
					if (args[2][0] == '+')
						state = true;
					else if (args[2][0] == '-')
						state = false;
					else
						return (false);
					if (args[2][1] == 'i') 
						channel.set_channel_limit(state);
					else if (args[2][1] == 't') {
						channel.set_channel_topic_state(state);
					}
					else if (args[2][1] == 'k') {
						if (state) {
							if (args.size() < 4)
								throw std::runtime_error(ERR_NEEDMOREPARAMS(client.get_nickname(), args[0]));
							if (args[3].find(' ') != std::string::npos)
								throw std::runtime_error(ERR_INVALIDKEY(client.get_nickname(), args[3]));
							channel.enable_key(args[3]);
						}
						else
							channel.disable_key();
					}
					else if (args[2][1] == 'o') {
						if (args.size() < 4)
							throw std::runtime_error(ERR_NEEDMOREPARAMS(client.get_nickname(), args[0]));
						Client& user = find_user(client.get_nickname(), args[3]);
						if (!channel.is_client_in_channel(user.get_socket()))
							throw std::runtime_error(ERR_USERNOTINCHANNEL(client.get_nickname(), user.get_nickname(), args[1]));
						if (state) {
							if (channel.is_user_operator(user.get_socket()))
								return (true);
							channel.promote_user_to_operator(user.get_socket());
							user.append_to_messages(RPL_YOUREOPER(user.get_nickname()));
						}
						else {
							if (!channel.is_user_operator(user.get_socket()))
								return (true);
							channel.demote_user_from_operator(user.get_socket());
						}
					}
					else if (args[2][1] == 'l') {
						if (state) {
							if (args.size() < 4)
								throw std::runtime_error(ERR_NEEDMOREPARAMS(client.get_nickname(), args[0]));
							try {
								int user_limit = std::stoi(args[3]);
								channel.set_user_limit(user_limit);
							}
							catch (std::out_of_range &e) {
								throw std::runtime_error(ERR_INVALIDMODEPARAM(client.get_nickname(), args[1], args[2][1], args[3], std::string("Input out of range.")));
							}
							catch(std::invalid_argument &e) {
								throw std::runtime_error(ERR_INVALIDMODEPARAM(client.get_nickname(), args[1], args[2][1], args[3], std::string("Input invalid.")));
							}
						}
						else
							channel.remove_user_limit();
					}

				}
				return (true);
			}
		},
	};
	auto command = command_map.find(args[0]);
	if (command != command_map.end()) {
		std::cout << "Client " << client.get_nickname() << " invoked: " << args[0] << std::endl;
		return (command->second());
	}
	else
		throw std::runtime_error(ERR_UNKNOWNCOMMAND(client.get_nickname(), args[0]));
	return (true);
}
