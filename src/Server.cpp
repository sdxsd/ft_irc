#include "lib/Server.hpp"
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <map>
#include <utility>
#include "lib/Executor.hpp"
#include "lib/utils.hpp"

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

void Server::pop_cmd(std::string &buf_string) {
	std::cout << "start string: " << buf_string << std::endl;
	size_t start = buf_string.find("\r\n");
	if (start == std::string::npos)
		return;
	buf_string = buf_string.substr((start + 2), (buf_string.size() - (start + 3)));
	std::cout << "remaining string:" << buf_string << std::endl;
}

void Server::handle_client(Client& client) {
	char		buf[BUFSIZE];
	std::string	buf_string;
	ssize_t bytes_read = recv(client.get_socket(), &buf, BUFSIZE, 0);
	if (bytes_read == 0 || bytes_read == -1) { // TODO: Separate -1 from 0, as one indicates an error.
		disconnect_client(client);
		return ;
	}

	if (buf_string.find("\n") != std::string::npos) {
		if (!client.get_recv_buffer().empty()) {
			client.append_to_recv_buffer(buf);
			buf_string = client.get_recv_buffer();
			client.clear_recv_buffer();
		}
		std::vector<std::string> split_cmd = split_by_delim(buf_string, ' ');
		execute(split_cmd, client, *this);
	}
	else {
		client.append_to_recv_buffer(buf_string);
	}
	// buf_string = buf;
	// while (buf_string.find("\n") != std::string::npos) { // NOTE: Switched to "\n" as "\r\n" is less common.
	// 	end = buf_string.find("\n");
	// 	command = buf_string.substr(0, end + 1);
	// 	std::cout << "command1: [" << command << "]" << std::endl;
	// 	// for (auto com : coms){
	// 	// 	if (command.find(com) != std::string::npos){
	// 	// 		size_t start = command.find(com);
	// 	// 		//std::cout << "found: " << start << std::endl;
	// 	// 		command = command.substr(start, (command.size() - start));
	// 	// 	}
	// 	// }
	// 	std::cout << "command2: [" << command << "]" << std::endl;
	// 	getCMD(command, client);
	// 	buf_string = buf_string.substr(end + 1, buf_string.size() - (end));
	// }
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

void Server::send_to_channel(const std::string& channel_name, const std::string &message) {
	Channel& channel = channels.find(channel_name)->second; // Get channel from channel name.
	for (auto c : channel.clients_in_channel()) // Loop through all clients in channel.
		c.second.append_to_messages(message); // Append message to clients stack of message to be sent.
}

// If using this function, check that the return value does not match clients.end()
// Client& Server::get_user(int fd)
// {
// 	return (clients.find(fd)->second);
// }

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
