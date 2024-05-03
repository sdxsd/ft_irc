#include "lib/Client.hpp"
#include <sys/socket.h>
#include <vector> 
#include <algorithm>
#include <string>
#include <iostream>

Client::Client(int client_sockfd): client_sockfd(client_sockfd) {
	;
}

Client::~Client() {
	;
}

const std::string& Client::get_recv_buffer() const {
	return (recv_buffer);
}

void Client::append_to_recv_buffer(const std::string& buf) {
	recv_buffer += buf;
}

void Client::send_message() {
	if (!messages.empty()) {
		const std::string& message = messages.top();
		if (send(client_sockfd, message.c_str(), message.length(), 0) != -1)
			messages.pop();
	}
}

void Client::replyPing(Client &client) {
	client.append_to_messages("PONG " + this->get_hostname() + "\r\n");
	client.send_message();
}

std::vector<std::string> split(const std::string& str, char delim) {
    std::vector<std::string> strings;
    size_t start;
    size_t end = 0;
    while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
        end = str.find(delim, start);
        strings.push_back(str.substr(start, end - start));
    }
    return strings;
}

void Client::storeNick(std::vector<std::string> &in, Client &client)
{
	std::reverse(in.begin(), in.end());	
	std::cout << "should be NICK: " << in[in.size() - 1] << std::endl;
	in.pop_back();
	std::cout << "nickname: " << in[in.size() - 1] << std::endl;
	if (!in[in.size()].empty())
		client.nickname = in[in.size() - 1];
	in.pop_back();
	std::reverse(in.begin(), in.end());	
	//std::cout << "quitting after Nick" << std::endl;
	//exit (0);
	// if (!in.empty())
	// 	client.nickname = in;
}

void Client::storeUserVals(std::vector<std::string> &in, Client &client){
	//std::cout << "this should be USER: " << in[0] << std::endl;
	std::reverse(in.begin(), in.end());
	//std::cout << "should be USER: " << in[in.size() - 1] << std::endl;
	in.pop_back();
	client.username = in[in.size() - 1];
	in.pop_back();
	client.hostname = in[in.size() - 1];
	in.pop_back();
	in.pop_back();
	while (!in[in.size() - 1].empty()){
		std::cout << "current: " << in[in.size() - 1] << std::endl;
		if (in[in.size() - 1].find("\r\n") != std::string::npos){
			client.realname.append(in[in.size() - 1]);
			//std::cout << "added(1): " << in[in.size() - 1] << std::endl; 
			in.pop_back();
			break ;
		}
		else
		{
			client.realname.append(in[in.size() - 1]);
			//std::cout << "added: " << in[in.size() - 1] << std::endl; 
			client.realname.append(" ");
			in.pop_back();
		}
	}
	client.realname = client.realname.substr(1, client.realname.size() - 1);
	std::reverse(in.begin(), in.end());
	std::cout << "remaining string: " << std::endl; 
	for (auto i: in)
		std::cout << i << " ";
	std::cout << std::endl;
	std::cout << "username: " << client.get_username() << std::endl;
	std::cout << "hostname: " << client.get_hostname() << std::endl;
	std::cout << "realname: " << client.get_realname() << std::endl;
	exit (0);
}


void Client::append_to_messages(const std::string& msg) {
	messages.push(msg);
}

int Client::get_socket() const {
	return (client_sockfd);
}

// std::string	Client::get_username() const{
// 	if (!this->username.empty())
// 		return (username);
// 	return ("none");
// }

// std::string	Client::get_nickname() const{
// 	if (!this->nickname.empty())
// 		return (nickname);
// 	return ("none");
// }

// std::string Client::get_hostname() const{
// 	if (!this->hostname.empty())
// 		return (hostname);
// 	return ("none");
// }

// std::string	Client::get_realname() const{
// 	if (!this->realname.empty())
// 		return (realname);
// 	return ("none");
// }

const int	Client::get_sockfd() const{
	return (client_sockfd);
}

const std::string& Client::get_username() const {
	return (username);
}

const std::string& Client::get_nickname() const {
	return (nickname);
}

const std::string& Client::get_hostname() const {
	return (hostname);
}

const std::string& Client::get_realname() const {
	return (realname);
}
