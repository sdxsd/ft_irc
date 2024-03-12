#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <stack>
#include <string>
#include <vector>

#define NICKNAME_LENGTH_MAX 9
#define BUFSIZE 512

class Client {
	private:
		const int				client_sockfd;
		std::string				username;
		std::string				nickname;
		std::string				hostname;
		std::string				realname;
		std::stack<std::string>	messages; // Messages waiting to be sent.
		std::string				recv_buffer; // Appended to until /r/n is encountered.
		std::string				send_buffer;
		bool					authenticated;
	public:
		// === Constructor & Destructor === //
		Client(int client_sockfd);
		~Client();
		// === Class Methods === //
		const std::string&	get_recv_buffer() const;
		void				append_to_recv_buffer(const std::string& buf);
		void				send_message();
		void				append_to_messages(const std::string& msg);
		int					get_socket() const;
};

#endif // CLIENT_HPP
