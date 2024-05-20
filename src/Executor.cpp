#include "lib/Server.hpp"
#include "lib/Replies.hpp"
#include <functional>
#include <stdexcept>
#include <iostream>

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
	static const std::map<std::string, std::function<int()>> command_map {
		{
			"CAP", [&]() -> int {
				sender.append_to_messages("421 CAP :No Cap\r\n");
				return (true);
			},
		},
		{
			"NICK", [&]() -> int {
				// TODO: Set nickname.
				sender.storeNick(args, sender);
				return (true);
			},
		},
		{
			"PASS", [&]() -> int {
				// TODO: Validate & set password.
				return (true);
			},
		},
		{
			"USER", [&]() -> int {
				if (!(args.size() > 1))
					return (false);
				sender.storeUserVals(args, sender);
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
				// TODO: part
				return (true);
			},
		},
		{
			"JOIN", [&]() -> int {
				std::cout << "Join command called." << std::endl;
				if (args[1][0] != ':') {
					throw std::runtime_error(ERR_BADCHANMASK(sender.get_nickname(), args[1]));
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
	if (command != command_map.end())
		command->second();
	else
		throw std::runtime_error(ERR_UNKNOWNCOMMAND(sender.get_nickname(), args[0]));
	return (true);
}
