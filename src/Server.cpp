#include "lib/Server.hpp"
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <functional>
#include <map>
#include <utility>

Server::Server(uint16_t port, const std::string &password): port(port), password(password)  {
	sockaddr_in	socket_address; // sockaddr_in represents an Internet Protocol address.

	memset(&socket_address, 0, sizeof(sockaddr_in)); // No random data.
	socket_address.sin_family = AF_INET; // Specifies that the socket communicates with IPv4 addresses.
	socket_address.sin_port = htons(port); // htons() converts port to network byte order.
	socket_address.sin_addr.s_addr = INADDR_ANY; // Accept connections from any address.
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sockfd == -1) {
		std::cerr << "Error initialising socket" << std::endl;
		exit(1); // TODO: Eventually remove.
	}
	if (bind(server_sockfd, (sockaddr*)&socket_address, sizeof(socket_address)) == -1) {
		std::cerr << "Error during binding of socket" << std::endl;
		close(server_sockfd);
		exit(1); // TODO: Eventually remove.
	}
	poll_sockfds.push_back({server_sockfd, POLLIN, 0});
}

std::vector<std::string> split_by_delim(std::string string, char delimiter) {
	std::vector<std::string> words;

	size_t pos = 0;
	while ((pos = string.find(delimiter)) != std::string::npos) {
		std::string word = string.substr(0, pos);
		words.push_back(word);
		string.erase(0, pos + 1);
	}
	return (words);
}

void Server::getCMD(std::string cmd_buf, Client& sender)
{
	const std::map<std::string, std::function<int(std::vector<std::string>)>> commands {
		{
			"CAP",
			[&](std::vector<std::string> args) -> int {
				if (args[0] == "CAP") {
					send(sender.get_socket(), "421 CAP :No Cap\r\n", 17, 0);
				}
				return (true);
			},
		},
		{
			"NICK",
			[&](std::vector<std::string> args) -> int {
				if (!(args.size() > 1))
					return (false);
				// TODO: Set nickname.
				sender.storeNick(args, sender);
				return (true);
			},
		},
		{
			"PASS",
			[&](std::vector<std::string> args) -> int {
				if (!(args.size() > 1))
					return (false);
				// TODO: Validate & set password.
				return (true);
			},
		},
		{
			"USER",
			[&](std::vector<std::string> args) -> int {
				if (!(args.size() > 1))
					return (false);
				// TODO: Populate user.
				sender.storeUserVals(args, sender);
				return (true);
			},
		},
		{
			"netcatter",
			[&](std::vector<std::string> args) -> int {
				if (!(args.size() > 1))
					return (false);
				// TODO: netcat?
				return (true);
			},
		},
		{
			"TOPIC",
			[&](std::vector<std::string> args) -> int {
				if (!(args.size() > 1))
					return (false);
				// TODO: topic
				return (true);
			},
		},
		{
			"PING",
			[&](std::vector<std::string> args) -> int {
				if (!(args.size() > 1))
					return (false);
				// TODO: ping
				return (true);
			},
		},
		{
			"PART",
			[&](std::vector<std::string> args) -> int {
				if (!(args.size() > 1))
					return (false);
				// TODO: part
				return (true);
			},
		},
		{
			"QUIT",
			[&](std::vector<std::string> args) -> int {
				if (!(args.size() > 1))
					return (false);
				// TODO: part
				return (true);
			},
		},
		{
			"JOIN",
			[&](std::vector<std::string> args) -> int {
				if (!(args.size() > 1))
					return (false);
				// TODO: join
				return (true);
			},
		},
		{
			"PRIVMSG",
			[&](std::vector<std::string> args) -> int {
				if (!(args.size() > 1))
					return (false);
				// TODO: PRIVMSG
				return (true);
			},
		},
		{
			"NOTICE",
			[&](std::vector<std::string> args) -> int {
				if (!(args.size() > 1))
					return (false);
				// TODO: notice
				return (true);
			},
		},
		{
			"INVITE",
			[&](std::vector<std::string> args) -> int {
				if (!(args.size() > 1))
					return (false);
				// TODO: invite
				return (true);
			},
		},
		{
			"KICK",
			[&](std::vector<std::string> args) -> int {
				if (!(args.size() > 1))
					return (false);
				// TODO: kick
				return (true);
			},
		},
		{
			"MODE",
			[&](std::vector<std::string> args) -> int {
				if (!(args.size() > 1))
					return (false);
				// TODO: mode
				return (true);
			},
		},
	};
	std::vector<std::string> splitArgs = split_by_delim(cmd_buf, ' ');
	// std::cout << "=== ARGS ===" << std::endl;
	// for (auto i : splitArgs)
	// 	std::cout << i << std::endl;
	// std::cout << "=== END ===" << std::endl;

	if (splitArgs.size() < 1)
		return ;
	else {
		auto command = commands.find(splitArgs[0]);
		if (command != commands.end())
			command->second(splitArgs);
	}
}

void Server::accept_new_client() {
	sockaddr_in	client_addr;
	int			sockfd;
	socklen_t	client_address_length = sizeof(client_addr);

	sockfd = accept(server_sockfd, (sockaddr*)&client_addr, &client_address_length);
	if (sockfd != -1) {
		std::cout << "Client connected!" << std::endl; // TODO: TEMP
		poll_sockfds.push_back({sockfd, POLLIN | POLLOUT, 0});
		clients.insert(std::make_pair(sockfd, Client(sockfd))); // Client ID represented by the file descriptor used to communicate with them.
		clients.find(sockfd)->second.append_to_messages("001 <name> :Welcome to the server!\r\n");
	}
}

void Server::send_to_channel(const std::string& channel_name, const std::string &message) {
	Channel& channel = channels.find(channel_name)->second; // Get channel from channel name.
	for (auto c : channel.clients_in_channel()) // Loop through all clients in channel.
		c.second.append_to_messages(message); // Append message to clients stack of message to be sent.
}

void Server::handle_client(Client& client) {
	char		buf[BUFSIZE];
	std::string	buf_string;
	ssize_t bytes_read = recv(client.get_socket(), &buf, BUFSIZE, 0);
	if (bytes_read == 0 || bytes_read == -1) { // TODO: Separate -1 from 0, as one indicates an error.
		disconnect_client(client);
		return ;
	}
	buf_string = buf;
	std::cout << "hostname: " << client.get_hostname() << std::endl;
	if (buf_string.find("\n") != std::string::npos) { // NOTE: Switched to "\n" as "\r\n" is less common.
		getCMD(buf_string, client);
	}
	else {
		;
	}
}

void Server::disconnect_client(Client &client) {
	std::cout << "Client disconnected." << std::endl;
	clients.erase(client.get_socket());
	for (auto it = poll_sockfds.begin(); it != poll_sockfds.end(); it++) {
		if (it->fd == client.get_socket()) {
			poll_sockfds.erase(it);
			break ;
		}
	}
	// TODO: Go through each channel also removing the user.
	return ;
}

// If using this function, check that the return value does not match clients.end()
Client& Server::get_user(int fd)
{
	return (clients.find(fd)->second);
}

void Server::run(void) {
	if (listen(server_sockfd, MAXCLIENT) == -1) {
		std::cerr << "Error setting socket to listen for connections." << std::endl;
		close(server_sockfd);
		exit(1); // TODO: Eventually remove.
	}
	std::cout << "Server listening on port: " << port << std::endl;
	while (true) { // TODO: Main logic (for now)
		int	presult = poll(poll_sockfds.data(), poll_sockfds.size(), -1);
		if (presult != -1 && presult >= 1) {
			for (const pollfd& pfd : poll_sockfds) {
				if (pfd.revents & POLLIN) {
					if (pfd.fd == server_sockfd)
						accept_new_client();
					else
						handle_client(clients.find(pfd.fd)->second);
				}
				else if (pfd.revents & POLLOUT) {
					clients.find(pfd.fd)->second.send_message();
				}
			}
		}
	}
}
