#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <poll.h>
#include <map>
#include "Client.hpp"
#include "Channel.hpp"

# define MAXCLIENT 1024

class Client;
class Channel;


class Server {
	private:
		int								server_sockfd;
		const uint16_t					port;
		std::map<int, Client>			clients;
		std::map<std::string, Channel>	channels;
		std::vector<pollfd>				poll_sockfds;
		const std::string				&password;
		void	getCMD(std::string cmd_buf, Client& sender);
		void	accept_new_client();
		void	send_to_channel(const std::string& channel_name, const std::string &message);
		void	handle_client(Client& client);
		void	disconnect_client(Client &client);
		void 	pop_cmd(std::string &buf_string);
		Client *getUser(int FD);
		Client& get_user(int fd);

	public:
		Server(uint16_t port, const std::string& password);
		void	run();
};

#endif // SERVER_HPP
