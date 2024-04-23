#include "lib/Server.hpp"
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream> 

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

void Server::send_to_channel(const std::string& channel_name, const std::string &message) {
	Channel& channel = channels.find(channel_name)->second; // Get channel from channel name.
	for (auto& c : channel.clients_in_channel()) // Loop through all clients in channel.
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
		getCMD(buf_string, &client);
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

void Server::getCMD(std::string cmd_buf, Client *sender)
{
	std::vector<std::string> splitArgs; 
	std::stringstream ss(cmd_buf);
	std::string word;
	while (ss >> word)
		splitArgs.push_back(word);
	size_t vecSize = splitArgs.size();
	if (sender == nullptr)
		throw std::runtime_error("invalid user");
	if (vecSize < 1)
		return ;
	else if (!splitArgs[0].compare("CAP")){
		send(sender->get_socket(), "421 CAP :No Cap\r\n", 17, 0);
	}
	// /else if ((vecSize > 1) && (!splitArgs[0].compare("PASS")))
		// validate password
	else if (!splitArgs[0].compare("NICK"))
		sender->storeNick(splitArgs[1], *sender);
	else if (!splitArgs[0].compare("USER"))
		// TODO: Check for valid user arguments and completenes
		sender->storeUserVals(splitArgs, *sender);
	else if ((vecSize > 1) && (!splitArgs[0].compare("netcatter")))
		std::cout << "netcatter command" << std::endl;
		//netcatter reply
	else if (!splitArgs[0].compare("TOPIC"))
		std::cout << "TOPIC command" << std::endl;
		//topic reply
	else if (!splitArgs[0].compare("PING")){
		sender->replyPing(*sender);
	}
	else if (!splitArgs[0].compare("PART"))
		std::cout << "PART command" << std::endl;
		//PART reply
	else if (!splitArgs[0].compare("QUIT"))
		std::cout << "QUIT command" << std::endl;
		//QUIT reply
	else if (vecSize < 2)
		return ;
	else if (!splitArgs[0].compare("JOIN"))
		std::cout << "JOIN command" << std::endl;
		//JOIN reply
	else if (!splitArgs[0].compare("PRIVMSG"))
		std::cout << "PRIVMSG command" << std::endl;
		//PRIVMSG reply
	else if (!splitArgs[0].compare("NOTICE"))
		std::cout << "NOTICE command" << std::endl;
		//NOTICE reply
	else if (!splitArgs[0].compare("INVITE"))
		std::cout << "INVITE command" << std::endl;
		//INVITE reply
	else if (!splitArgs[0].compare("KICK"))
		std::cout << "KICK command" << std::endl;
		//KICK reply
	else if (!splitArgs[0].compare("MODE"))
		std::cout << "MODE command" << std::endl;
		//MODE reply	
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
