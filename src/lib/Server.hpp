#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstdint>
#include <string>
#include <vector>
#include "Channel.hpp"

#define MSG_BUFFER 512; // Maximum size of an IRC message.

class Server {
	private:
		int					server_sock;
		std::vector<int>	clients;
		const uint16_t		port;
		const std::string	name;
		std::vector<Channel> channels;
	public:
		Server(uint16_t port, const std::string& name);
		void	run();
		int		accept_new_clients();
};

#endif // SERVER_HPP
