#include "lib/Server.hpp"
#include <iostream>

/*
0 ... 1023 	Well-Known Ports(0-1023)
1433 	Microsoft SQL Server
1521 	Oracle Database
3306 	MySQL Database
5000 	UPnP (Universal Plug and Play) control point
5222 	XMPP (Extensible Messaging and Presence Protocol)
5432 	PostgreSQL Database
7070 	Real-Time Messaging Protocol (RTMP)
8080 	Alternate HTTP (commonly used for web servers)
8443 	HTTPS Alternate (often used for secure web traffic)
9090 	Alternate Web Server (often used for web proxy servers)
*/

std::string get_string(char *in){
	int i = 0; 
	while (in[i] != '\0')
		i++;
	return(std::string(in, i));
}

int	check_input(int count, char *prog, char *port, char *pwd){
	int ok = 1;
	std::string response = "";
	if (count != 3){
		std::cerr << "Usage: " << prog << " <PORT> <PASSWORD>" << std::endl;
		return (0);
	}
	uint16_t port_i = std::stoi(port);
	std::string pwd_str = get_string(pwd);
	if ((port_i > 0 && port_i <= 1023) || port_i > 10000){
		std::cerr << "This port is not allowed, please try again"  << std::endl;
		ok = 0;
	}
	if (pwd_str.length() > 500){
		std::cerr << "pwd is too long" << std::endl;
		ok = 0;
	}
	return(ok);
}

int main(int argc, char *argv[]) {
	if (!check_input(argc, argv[0], argv[1], argv[2]))
		return (1);
	const uint16_t port = std::stoi(argv[1]);
	const std::string password = std::string(argv[2]);
	Server server = Server(port, password);
	server.run();
}
