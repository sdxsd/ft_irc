#include "lib/Server.hpp"
#include "lib/Replies.hpp"
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
int Server::execute_cmd(std::vector<std::string>& args, Client& sender) {
	const std::map<std::string, std::function<int()>> command_map {
		{
			"CAP", [&]() -> int {
				if (args.size() > 2 && args[1] == "LS" && args[2] == "302")
					sender.append_to_messages(":localhost CAP * LS :\r\n");
				return (true);
			},
		},
		{
			"NICK", [&]() -> int {
				if (args.size() != 2)
					throw std::runtime_error(ERR_NONICKNAMEGIVEN(sender.get_hostname()));
				if (args[1][0] == '#' || args[1][0] == '&' || args[1][0] == ':' || args[1][0] == ' ')
					throw std::runtime_error(ERR_ERRONEUSNICKNAME(sender.get_nickname(), args[1]));
				for (std::pair<const int, Client>& c : clients)
					if (c.second.get_nickname() == args[1])
						throw std::runtime_error(ERR_NICKNAMEINUSE(sender.get_nickname(), args[1]));
				sender.set_nickname(args[1]);
				return (true);
			},
		},
		{
			"PASS", [&]() -> int {
				if (args.size() != 2)
					throw std::runtime_error(ERR_NEEDMOREPARAMS(sender.get_nickname(), args[0]));
				if (sender.is_registered())
					throw std::runtime_error(ERR_ALREADYREGISTERED(sender.get_nickname()));
				if (args[1] != password)
					throw std::runtime_error(ERR_PASSWDMISMATCH(sender.get_nickname()));
				return (true);
			},
		},
		{
			"USER", [&]() -> int {
				if (!(args.size() > 1))
					return (false);
				sender.register_client(args);
				sender.append_to_messages(RPL_WELCOME(sender.get_nickname(), sender.get_nickname()));
				// sender.append_to_messages("001 zoe :Welcome to the server!\r\n");
				return (true);
			},
		},
		{
			"netcatter", [&]() -> int {
				// TODO: netcat?
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
				// TODO: ping
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
				for (auto& p : channels) {
					Channel& c = p.second;
					if (c.clients_in_channel().find(sender.get_socket()) != c.clients_in_channel().end()) {
						for (auto& client : c.clients_in_channel()) {
							if (client.second.get_socket() != sender.get_socket())
								client.second.append_to_messages(RPL_QUIT(sender.get_nickname(), ""));
						}
					}
				}
				disconnect_client(sender);
				return (true);
			},
		},
		{
			"JOIN", [&]() -> int {
				if (args.size() < 2)
					throw std::runtime_error(ERR_NEEDMOREPARAMS(sender.get_nickname(), args[0]));
				if (!sender.is_registered())
					throw std::runtime_error(ERR_NOTREGISTERED(sender.get_nickname()));
				auto channel = channels.find(args[1]);
				if (channel != channels.end()) {
					channel->second.add_client_to_channel(sender.get_socket(), sender);
					std::cout << "Client has been added to channel" << std::endl;
				}
				else {
					channels.insert(std::make_pair(args[1], Channel(args[1], "", {}))); // TODO: MAKE SURE MODE IS NOT FUCKING EMPTY.
					std::cout << "Channel successfully created" << std::endl;
				}
				return (true);
			},
		},
		{
			"PRIVMSG", [&]() -> int {
				// TODO: PRIVMSG
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
		command->second();
	}
	else
		throw std::runtime_error(ERR_UNKNOWNCOMMAND(sender.get_nickname(), args[0]));
	return (true);
}
