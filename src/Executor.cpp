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
				if (args[1] != password) {
					client.append_to_messages(ERR_PASSWDMISMATCH(client.get_nickname()));
					disconnect_client(client);
					return (false);
				}
				return (true);
			},
		},

		{
			"USER", [&]() -> int {
				if (!(args.size() > 1))
					return (false);
				client.register_client(args);
				client.append_to_messages(RPL_WELCOME(client.get_nickname(), client.get_nickname()));
				return (true);
			},
		},

		{
			"TOPIC", [&]() -> int {
				// TODO: topic
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
				// TODO: part
				return (true);
			},
		},

		{
			"QUIT", [&]() -> int {
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
				if (!client.is_registered())
					throw std::runtime_error(ERR_NOTREGISTERED(client.get_nickname()));
				auto channel = channels.find(args[1]);
				if (channel != channels.end()) {
					channel->second.add_client_to_channel(client);
					std::cout << "Client has been added to channel." << std::endl;
				}
				else {
					channels.insert({args[1], Channel(args[1], "", {})}); // TODO: MAKE SURE MODE IS NOT FUCKING EMPTY.
					auto channel = channels.find(args[1]);
					channel->second.add_client_to_channel(client);
					std::cout << "Channel successfully created" << std::endl;
				}
				return (true);
			},
		},

		{
			"PRIVMSG", [&]() -> int { // NOTE: In the interest of a simpler parsing system we have chosen to disallow multiple target messages.
				if (!client.is_registered())
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
						channel->second.echo_message_to_channel(client.get_socket(), RPL_PRIVMSG(client.get_nickname(), target, msg));
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
				// TODO: notice
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
