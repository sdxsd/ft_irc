#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <poll.h>
#include "Channel.hpp"

#define MSG_BUFFER 512; // Maximum size of an IRC message.

class Server {
	private:
		int								server_sockfd;
		const uint16_t					port;
		std::map<unsigned int, Client>	clients;
		std::map<std::string, Channel>	channels;
		std::vector<pollfd>				poll_sockfds;
		const std::string				&password;
		void	accept_new_client();
	public:
		Server(uint16_t port, const std::string& password);
		void	run();
};

#endif // SERVER_HPP
