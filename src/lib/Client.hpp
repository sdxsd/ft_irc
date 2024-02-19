#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

#define NICKNAME_LENGTH_MAX 9

class Client {
	private:
		const int client_sock;
		std::string recv_buffer;
		std::string send_buffer;
		std::string username;
		std::string nickname;
		std::string hostname;
		std::string realname;
	public:
		Client(int client_sock);
		int	get_socket() const;
};

#endif // CLIENT_HPP
