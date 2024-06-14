#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <poll.h>
#include <map>
#include "Client.hpp"
#include "Channel.hpp"

#define MAXCLIENT 1024

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
		std::vector<std::string> *read_from_client(Client& client);
		void	handle_client(Client& client);
		void	disconnect_client(Client &client);
		int		execute_cmd(std::vector<std::string>& args, Client& sender); // Implemented in Executor.hpp
		Client& find_user(const std::string& sender_nick, const std::string& nick);
		Channel& find_channel(const std::string& nick, const std::string &channel_name);
	public:
		Server(uint16_t port, const std::string& password);
		void	run();
};

#endif // SERVER_HPP
