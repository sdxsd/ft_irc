#include "lib/Server.hpp"
#include "lib/Replies.hpp"
#include "lib/utils.hpp"
#include <cstddef>
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
				if (client.has_valid_password() == false) { // TODO: Probably need to inform the client the password is wrong...
					disconnect_client(client);
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
				if (client.is_registered()) // TODO: Determine if this is needed.
					throw std::runtime_error(ERR_ALREADYREGISTERED(client.get_nickname()));
				std::cout << "Password set: "<< args[1] << std::endl;
				if (args[1] != password) {
					client.append_to_messages(ERR_PASSWDMISMATCH(client.get_nickname()));
					client.send_message();
					disconnect_client(client);
					return (false);
				}
				else
					client.set_password_validity(true);
				return (true);
			},
		},

		{
			"USER", [&]() -> int {
				if (client.has_valid_password() == false) { // TODO: Probably need to inform the client the password is wrong...
					disconnect_client(client);
					return (false);
				}
				client.register_client(args);
				client.append_to_messages(RPL_WELCOME(client.get_nickname(), client.get_nickname()));
				return (true);
			},
		},

		{
			"TOPIC", [&]() -> int { // FIXME: Implement topic changing.
				if (args.size() < 2)
					throw std::runtime_error(ERR_NEEDMOREPARAMS(client.get_nickname(), args[0]));
				auto channel = channels.find(args[1]);
				if (channel == channels.end())
					throw std::runtime_error(ERR_NOSUCHCHANNEL(client.get_nickname(), args[1]));
				if (!(channel->second.is_client_in_channel(client.get_socket())))
					throw std::runtime_error(ERR_NOTONCHANNEL(client.get_nickname(), args[1]));
				if (channel->second.channel_has_topic())
					client.append_to_messages(RPL_TOPIC(client.get_nickname(), args[1], channel->second.get_topic()));
				else
					client.append_to_messages(RPL_NOTOPIC(client.get_nickname(), args[1]));
				return (true);
			},
		},
		{
			"PING", [&]() -> int {
				client.append_to_messages(RPL_PING(std::string("localhost"), args[1]));
				return (true);
			},
		},
		{
			"PART", [&]() -> int {
				if (args.size() < 2)
					throw std::runtime_error(ERR_NEEDMOREPARAMS(client.get_nickname(), args[0]));
				if (args[1][0] != '#')
					throw std::runtime_error(ERR_BADCHANMASK(client.get_nickname(), args[1]));
				auto channel = channels.find(args[1]);
				if (channel == channels.end())
					throw std::runtime_error(ERR_NOSUCHCHANNEL(client.get_nickname(), args[1]));
				if (!(channel->second.is_client_in_channel(client.get_socket())))
					throw std::runtime_error(ERR_NOTONCHANNEL(client.get_nickname(), args[1]));
				std::string reason = "";
				if (args.size() > 2) {
					for (unsigned long i = 2; i < args.size(); i++)
						reason += (args[i] + " ");
					reason = trimWhitespace(reason);
					channel->second.echo_message_to_channel(RPL_PART(client.get_hostmask(), args[1], reason));
				}
				else
					channel->second.echo_message_to_channel(RPL_PART(client.get_hostmask(), args[1], reason));
				channel->second.remove_client_from_channel(client);
				return (true);
			},
		},

		{
			"QUIT", [&]() -> int {
				std::string msg = (args.size() > 1) ? args[1] : "No reason";
				for (auto& channel : channels) {
					if (channel.second.is_client_in_channel(client.get_socket()) == true) {
						channel.second.echo_message_to_channel(RPL_QUIT(client.get_nickname(), msg));
					}
				}
				disconnect_client(client);
				return (false);
			},
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
					channel->second.add_client_to_channel(client);
					std::cout << "Client has been added to channel." << std::endl;
				}
				else {
					channels.insert({args[1], Channel(args[1], "", {})}); // TODO: MAKE SURE MODE IS NOT FUCKING EMPTY.
					channel = channels.find(args[1]);
					channel->second.add_client_to_channel(client);
					std::cout << "Channel successfully created" << std::endl;
				}
				client.append_to_messages(RPL_JOIN(client.get_hostmask(), args[1]));
				if (channel->second.channel_has_topic())
					client.append_to_messages(RPL_TOPIC(client.get_nickname(), args[1], channel->second.get_topic()));
				for (auto& c : channel->second.clients_in_channel())
					if (c.first != client.get_socket())
						client.append_to_messages(RPL_NAMREPLY(client.get_nickname(), args[1], c.second->get_nickname()));
				client.append_to_messages(RPL_ENDOFNAMES(client.get_nickname(), args[1]));
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
					auto channel = channels.find(target);
					if (channel != channels.end())
						channel->second.echo_privmsg_to_channel(client.get_socket(), RPL_PRIVMSG(client.get_nickname(), target, msg));
					else
						throw std::runtime_error(ERR_NOSUCHCHANNEL(client.get_nickname(), args[1]));
				}
				else { // Sending to user.
					Client *receiver = find_user(target);
					if (receiver == NULL)
						throw std::runtime_error(ERR_NOSUCHNICK(client.get_nickname(), target));
					receiver->append_to_messages(RPL_PRIVMSG(client.get_nickname(), target, msg));
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
				// TODO: invite
				return (true);
			},
		},
		{
			"KICK", [&]() -> int {
				// TODO: kick
				return (true);
			},
		},
		{
			"MODE", [&]() -> int {
				// TODO: mode
				return (true);
			},
		},
	};
	auto command = command_map.find(args[0]);
	if (command != command_map.end()) {
		std::cout << "Client invoked: " << args[0] << std::endl;
		return (command->second());
	}
	else
		throw std::runtime_error(ERR_UNKNOWNCOMMAND(client.get_nickname(), args[0]));
	return (true);
}
