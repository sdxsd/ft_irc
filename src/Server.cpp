#include "lib/Server.hpp"
#include <cstring>
#include <sys/socket.h>

Server::Server(uint16_t port, const std::string &name): _port(port), _name(name)  {
	sockaddr addr;
	addr.sa_family = AF_INET;
	strncpy(addr.sa_data, name.c_str(), sizeof(addr.sa_data) - 1);
	_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (bind(_sock_fd, &addr, sizeof(sockaddr)) == -1) {
		exit(1);
	}
}
