#include "lib/Server.hpp"
#include <cstdlib>
#include "lib/Replies.hpp"
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
	int			sockfd;
	sockaddr_in	client_addr;
	socklen_t	client_address_length = sizeof(client_addr);

	sockfd = accept(server_sockfd, (sockaddr*)&client_addr, &client_address_length);
	if (sockfd != -1) {
		std::cout << "Client connected!" << std::endl; // TODO: TEMP
		poll_sockfds.push_back({sockfd, POLLIN | POLLOUT, 0});
		clients.insert({sockfd, Client(sockfd)}); // Client ID represented by the file descriptor used to communicate with them.
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
	return (split(buf_string, "\r\n"));
}

void Server::handle_client(Client& client) {
	std::vector<std::string> *lines = read_from_client(client);
	if (lines == NULL)
		return ;
	for (auto l : *lines) {
		std::vector<std::string> *tokens = split(l, " ");
		std::cout << l << std::endl;
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
	// TODO: Need to store incomplete messages in the damn recv buffer, not a problem with IRSSI.
	// 	client.append_to_recv_buffer(buf_string);
}

Client *Server::find_user(const std::string& nick) {
	for (auto& c : clients) {
		if (c.second.get_nickname() == nick)
			return (&c.second);
	}
	return (NULL);
}

void Server::disconnect_client(Client &client) {
	std::cout << "Client " << client.get_nickname() << " disconnected." << std::endl;
	close(client.get_socket());
	for (auto it = poll_sockfds.begin(); it != poll_sockfds.end(); it++) {
		if (it->fd == client.get_socket()) {
			poll_sockfds.erase(it);
			break ;
		}
	}
	for (auto& p : channels) {
		if (p.second.is_client_in_channel(client.get_socket())) {
			for (auto& clientoids : p.second.clients_in_channel())
				if (clientoids.first != client.get_socket())
					clientoids.second->append_to_messages(RPL_QUIT(client.get_nickname(), ""));
			if (p.second.is_user_operator(client.get_socket()) == true)
				p.second.demote_user_from_operator(client.get_socket());
			p.second.remove_client_from_channel(client);
		}
	}
	clients.erase(client.get_socket());
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
				else if (pfd.revents & POLLOUT)
					clients.find(pfd.fd)->second.send_message();
			}
		}
	}
}
