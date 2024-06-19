#include "lib/Server.hpp"
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <strings.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <map>
#include <utility>
#include "lib/Replies.hpp"
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
	int iSetOption = 1;
	setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof(iSetOption));
	if (bind(server_sockfd, (sockaddr*)&socket_address, sizeof(socket_address)) == -1) {
		std::cerr << "Error during binding of socket" << std::endl;
		close(server_sockfd);
		exit(1); // TODO: Eventually remove.
	}
	poll_sockfds.push_back({server_sockfd, POLLIN, 0});
}

void Server::accept_new_client() {
	int			sockfd;
	sockaddr_in	client_addr;
	socklen_t	client_address_length = sizeof(client_addr);

	sockfd = accept(server_sockfd, (sockaddr*)&client_addr, &client_address_length);
	if (sockfd != -1) {
		std::cout << "Client connected!" << std::endl;
		poll_sockfds.push_back({sockfd, POLLIN | POLLOUT, 0});
		clients.insert(std::make_pair(sockfd, Client(sockfd))); // Client ID represented by the file descriptor used to communicate with them
	}
}

std::vector<std::string> *Server::read_from_client(Client& client) {
	char buf[BUFSIZE];
	for (int i = 0; i < BUFSIZE; i++)
		buf[i] = '\0';
	ssize_t bytes_read = recv(client.get_socket(), &buf, BUFSIZE - 1, 0);
	if (bytes_read == 0 || bytes_read == -1) { // TODO: Separate -1 from 0, as one indicates an error.
		disconnect_client(client);
		return (NULL);
	}
	std::string buf_string = buf;
	if (!client.get_recv_buffer().empty()) {
		buf_string = (client.get_recv_buffer() + buf_string);
		client.clear_recv_buffer();
	}
	std::string delimiter = get_delimiter(buf_string);
	if (delimiter.empty()) {
		std::cout << "No delim" << std::endl;
		client.append_to_recv_buffer(buf_string);
		return (NULL);
	}
	std::vector<std::string> *commands = split(buf_string, delimiter);
	if (!commands)
		return (NULL);
	if (!last_contains_delimiter(buf_string)) {
		client.append_to_recv_buffer(commands->back());
		commands->pop_back();
	}
	return (commands);
}

void Server::handle_client(Client& client) {
	std::vector<std::string> *lines = read_from_client(client);
	if (lines == NULL)
		return ;
	for (auto l : *lines) {
		std::vector<std::string> *tokens = split(l, " ");
		std::cout << "From: " << l << std::endl;
		for (std::string& s : *tokens)
			s = trimWhitespace(s);
		try {
			if (execute_cmd(*tokens, client) == false) {
				delete lines;
				delete tokens;
				return ;
			}
		}
		catch (std::runtime_error &e) {
			client.append_to_messages(e.what());
		}
		delete tokens;
	}
	delete lines;
}

Client &Server::find_user(const std::string& sender_nick, const std::string& nick) {
	for (auto& c : clients)
		if (c.second.get_nickname() == nick)
			return (c.second);
	throw std::runtime_error(ERR_NOSUCHNICK(sender_nick, nick));
}

void Server::disconnect_client(Client &client) {
	std::vector<std::string> channels_to_erase;
	close(client.get_socket());
	for (auto it = poll_sockfds.begin(); it != poll_sockfds.end(); it++) {
		if (it->fd == client.get_socket()) {
			poll_sockfds.erase(it);
			break ;
		}
	}
	for (auto& p : channels) {
		if (p.second.is_client_in_channel(client.get_socket())) {
			if (p.second.is_user_operator(client.get_socket()) == true)
				p.second.demote_user_from_operator(client.get_socket());
			p.second.remove_client_from_channel(client);
			if (p.second.clients_in_channel().size() < 1)
				channels_to_erase.push_back(p.first);
		}
	}
	for (std::string c : channels_to_erase)
		channels.erase(c);
	std::cout << "Client " << client.get_nickname() << " disconnected." << std::endl;
	clients.erase(client.get_socket());
}

Channel& Server::find_channel(const std::string& nick, const std::string& channel_name) {
	auto channel = channels.find(channel_name);
	if (channel == channels.end())
		throw std::runtime_error(ERR_NOSUCHCHANNEL(nick, channel_name));
	return (channel->second);
}

void Server::run(void) {
	if (listen(server_sockfd, MAXCLIENT) == -1) {
		std::cerr << "Error setting socket to listen for connections." << std::endl;
		close(server_sockfd);
		exit(1); // TODO: Eventually remove.
	}
	std::cout << "Server listening on port: " << port << std::endl;
	while (true) {
		int	presult = poll(poll_sockfds.data(), poll_sockfds.size(), -1);
		if (presult != -1 && presult >= 1) {
			for (const pollfd& pfd : poll_sockfds) {
				if (pfd.revents & POLLIN) {
					if (pfd.fd == server_sockfd)
						accept_new_client();
					else {
						auto c = clients.find(pfd.fd);
						if (c != clients.end())
							handle_client(clients.find(pfd.fd)->second);
					}
				}
				else if (pfd.revents & POLLOUT) {
					auto c = clients.find(pfd.fd);
					if (c != clients.end()) {
						c->second.send_message();
						if (c->second.messages_queue_size() == 0 && c->second.awaiting_disconnect()) {
							c->second.append_to_messages("ERROR: " + c->second.get_disconnect_reason());
							c->second.send_message();
							disconnect_client(c->second);
						}
					}
				}
			}
		}
	}
}
