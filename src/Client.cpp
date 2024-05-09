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
	std::string msg = "Pong " + client.get_hostname() + "\r\n";
	client.append_to_messages(msg);
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
	//std::cout << "should be NICK: " << in[in.size() - 1] << std::endl;
	in.pop_back();
	//std::cout << "nickname: " << in[in.size() - 1] << std::endl;
	if (!in[in.size() - 1].empty()){
		client.nickname = in[in.size() - 1];
		in.pop_back();
	}
	//std::cout << "stored nickname: " <<get_nickname() << std::endl; 
	std::reverse(in.begin(), in.end());	
}

void Client::storeUserVals(std::vector<std::string> &in, Client &client){
	//std::cout << "this should be USER: " << in[0] << std::endl;
	std::cout << "buffer at start: ";
	std::reverse(in.begin(), in.end());
	//std::cout << "should be USER: " << in[in.size() - 1] << std::endl;
	in.pop_back();
	client.username = in[in.size() - 1];
	in.pop_back();
	client.hostname = in[in.size() - 1];
	in.pop_back();
	in.pop_back();;
	int pos = in.size();
	while (pos > -1 && !in[pos].empty()){
		std::cout << "current: " << in[pos] << std::endl;
		if (in[pos].find("\r\n") != std::string::npos){
			std::cout << "found end at: " << in[pos] << std::endl;
			client.realname.append(in[pos]);
			in.pop_back();
			break ;
		}
		else
		{
			client.realname.append(in[pos]);
			client.realname.append(" ");
			in.pop_back();
		}
		pos--;
	}
	client.realname = client.realname.substr(1, client.realname.size() - 1);
	std::reverse(in.begin(), in.end());
	std::cout << "remaining string: " << std::endl;
	int size = in.size(); 
	for(int i = 0; i <= size; i++)
		std::cout << in[i] << " ";
	std::cout << std::endl;
	std::cout << "nickname: " << client.get_nickname() << std::endl;
	std::cout << "username: " << client.get_username() << std::endl;
	std::cout << "hostname: " << client.get_hostname() << std::endl;
	std::cout << "realname: " << client.get_realname() << std::endl;
}


void Client::append_to_messages(const std::string& msg) {
	messages.push(msg);
}

int Client::get_socket() const {
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
