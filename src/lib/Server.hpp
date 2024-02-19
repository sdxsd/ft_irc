#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstdint>
#include <string>
#include <vector>
#include "Channel.hpp"

#define MSG_BUFFER 512; // Maximum size of an IRC message.

class Server {
	private:
		int								server_sock;
		const uint16_t					port;
		std::map<unsigned int, Client>	clients;
		std::map<std::string, Channel>	channels;
		const std::string				&password;
	public:
		Server(uint16_t port, const std::string& password);
		void	run();
		int		accept_new_clients();
};

#endif // SERVER_HPP
