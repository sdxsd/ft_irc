#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <stack>
#include <string>

#define NICKNAME_LENGTH_MAX 9

class Client {
	private:
		const int				client_sockfd;
		std::string				recv_buffer;
		std::string				send_buffer;
		std::string				username;
		std::string				nickname;
		std::string				hostname;
		std::string				realname;
		std::stack<std::string> messages;
	public:
		Client(int client_sockfd);
		int		get_socket() const;
		void	send_message();
};

#endif // CLIENT_HPP
