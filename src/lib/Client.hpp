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
		std::string				old_nickname;
		std::string				nickname;
		std::string				hostname;
		std::string				realname;
		std::string				servername;
		std::stack<std::string>	messages; // Messages waiting to be sent.
		std::string				recv_buffer; // Appended to until /n is encountered.
		std::string				send_buffer;
		bool					registered;
		bool					password_valid;
	public:
		// === Constructor & Destructor === //
		Client(int sockfd);
		~Client();
		// === Class Methods === //
		void				append_to_recv_buffer(const std::string& buf);
		void				clear_recv_buffer();
		void				send_message();
		void				replyPing(Client &client);
		void				append_to_messages(const std::string& msg);
		int					get_socket() const;
		void 				register_client(std::vector<std::string> &args);
		void				set_nickname(std::string nick);
		void				storePWD(std::string &in, Client &client);
		std::string			get_hostmask() const;
		std::string			get_recv_buffer();
		const std::string&	get_username() const;
		const std::string&	get_nickname() const;
		const std::string&  get_old_nickname() const;
		const std::string&	get_hostname() const;
		const std::string&	get_realname() const;
		bool				is_registered() const;
		bool				has_valid_password() const;
		void				set_password_validity(bool state);
		bool 				is_valid_client() const;
};

#endif // CLIENT_HPP
