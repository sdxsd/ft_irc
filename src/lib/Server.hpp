#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstdint>
#include <string>
#include <vector>
#include "Channel.hpp"

class Server {
	private:
		int					_sock_fd;
		const uint16_t		_port;
		const std::string	_name;
		std::vector<Channel> _channels;
	public:
		Server(uint16_t port, const std::string& name);
};

#endif // SERVER_HPP
