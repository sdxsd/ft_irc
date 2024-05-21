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
				for (std::pair<const int, Client>& c : clients)
					if (c.second.get_nickname() == args[1])
						throw std::runtime_error(ERR_NICKNAMEINUSE(client.get_nickname(), args[1]));
				client.set_nickname(args[1]);
				return (true);
			},
		},
		{
			"PASS", [&]() -> int {
				if (args.size() != 2)
					throw std::runtime_error(ERR_NEEDMOREPARAMS(client.get_nickname(), args[0]));
				if (client.is_registered())
					throw std::runtime_error(ERR_ALREADYREGISTERED(client.get_nickname()));
				if (args[1] != password)
					throw std::runtime_error(ERR_PASSWDMISMATCH(client.get_nickname()));
				return (true);
			},
		},
		{
			"USER", [&]() -> int {
				if (!(args.size() > 1))
					return (false);
				client.register_client(args);
				client.append_to_messages(RPL_WELCOME(client.get_nickname(), client.get_nickname()));
				// client.append_to_messages("001 zoe :Welcome to the server!\r\n");
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
				client.append_to_messages("PING :server :\r\n");
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
					if (p.second.is_client_in_channel(client.get_socket())) {
						for (auto& clientoids : p.second.clients_in_channel()) {
							if (clientoids.second.get_socket() != client.get_socket())
								clientoids.second.append_to_messages(RPL_QUIT(client.get_nickname(), ""));
						}
					}
				}
				disconnect_client(client);
				return (true);
			},
		},
		{
			"JOIN", [&]() -> int {
				if (args.size() < 2)
					throw std::runtime_error(ERR_NEEDMOREPARAMS(client.get_nickname(), args[0]));
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
			"PRIVMSG", [&]() -> int {
				if (!client.is_registered())
					throw std::runtime_error(ERR_NOTREGISTERED(client.get_nickname()));
				auto channel = channels.find(args[1]); // FIXME: Messages need to be sent to users too.
				if (channel != channels.end()) {
					auto& cloids = channel->second.clients_in_channel();
					for (auto& c : cloids) {
						if (c.first != client.get_socket()) // FIXME: Currently only sending the first word...
							c.second.append_to_messages(RPL_PRIVMSG(client.get_nickname(), args[1], args[2]));
					}
				}
				else
					throw std::runtime_error(ERR_NOSUCHCHANNEL(client.get_nickname(), args[1]));
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
		throw std::runtime_error(ERR_UNKNOWNCOMMAND(client.get_nickname(), args[0]));
	return (true);
}
