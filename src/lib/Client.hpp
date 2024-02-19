#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

#define NICKNAME_LENGTH_MAX 9

class Client {
	private:
	  int client_sock;
	  char recv_buffer[512];
	  char send_buffer[512];
	  const std::string username;
	  const std::string nickname;
	  const std::string hostname;
	  const std::string realname;
	public:
	  Client(int client_sock);
};

#endif // CLIENT_HPP
