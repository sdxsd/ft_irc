#include "lib/Server.hpp"
#include <iostream>
#include <cstring>

int main(int argc, char *argv[]) {
	if (argc != 3)
		std::cerr << "Usage: " << argv[0] << " <PORT> <PASSWORD>" << std::endl;
	const uint16_t port = std::stoi(argv[1]);
	const std::string password = std::string(argv[2]);
	Server server = Server(port, password);
	server.run();
}
