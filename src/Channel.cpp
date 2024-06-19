#include "lib/Channel.hpp"
#include "lib/Replies.hpp"
#include <iostream>
#include <algorithm>
#include <stdexcept>

Channel::Channel(const std::string &name):
name(name) {
	if (name[0] != '#')
		throw std::runtime_error(ERR_BADCHANMASK(std::string("something"), name));
	for (const char& c : "\7 ") // Check for illegal characters within name.
		if (name.find(c) != std::string::npos)
			throw std::runtime_error(ERR_BADCHANMASK(std::string("something"), name));
	password_protected.second = "";
	topic.second = "";
}

std::map<int, Client*>& Channel::clients_in_channel() {
	return (clients);
}

bool Channel::user_invited(int user){
	for (auto i : invite_only.second){
		if (i == user)
			return (true);
	}
	return (false);
}

const std::vector<int>& Channel::operators_in_channel() {
	return (operators);
}

bool Channel::is_client_in_channel(int client_sockfd) {
	return ((clients.find(client_sockfd) != clients.end()));
}

void Channel::add_client_to_channel(Client& client) {
	if (!is_client_in_channel(client.get_socket())) {
		clients.insert({client.get_socket(), &client});
		std::cout << "Added client: " << client.get_nickname() << " To: " << name << std::endl;
	}
	else
		throw std::runtime_error(ERR_USERONCHANNEL(client.get_nickname(), client.get_nickname(), name));
}

void Channel::remove_client_from_channel(const Client& client) {
	clients.erase(client.get_socket());
}

int Channel::get_user_limit() {
	return (user_limit.second);
}

void Channel::set_user_limit(int user) {
	user_limit.first = true;
	user_limit.second = user;
}

void Channel::remove_user_limit() {
	user_limit.first = false;
	user_limit.second = 0;
}

bool Channel::user_limit_set() {
	return (user_limit.first);
}

void Channel::disable_key() {
	password_protected.first = false;
	password_protected.second.clear();
}

void Channel::enable_key(const std::string& key) {
	password_protected.first = true;
	password_protected.second = key;
}

bool Channel::is_key_required() {
	return (password_protected.first);
}

bool Channel::validate_key(const std::string& key) {
	if (!password_protected.first)
		return (true);
	if (key == password_protected.second)
		return (true);
	return (false);
}

std::string Channel::get_mode() const {
	std::string modestring = "+";
	if (invite_only.first)
		modestring += 'i';
	if (password_protected.first)
		modestring += 'k';
	if (topic.first)
		modestring += 't';
	if (user_limit.first)
		modestring += 'l';
	return (modestring);
}

void Channel::echo_privmsg_to_channel(int sender_fd, const std::string& msg) {
	for (auto& c : clients)
		if (c.first != sender_fd)
			c.second->append_to_messages(msg);
}

void Channel::echo_message_to_channel(const std::string& msg) {
	for (auto& c : clients)
			c.second->append_to_messages(msg);
}

bool Channel::is_user_operator(int fd) {
	if (std::find(operators.begin(), operators.end(), fd) != operators.end())
		return (true);
	return (false);
}

void Channel::promote_user_to_operator(int fd) {
	if (std::find(operators.begin(), operators.end(), fd) == operators.end())
		operators.push_back(fd);
}

void Channel::demote_user_from_operator(int fd) {
	auto oper = std::find(operators.begin(), operators.end(), fd);
	if (oper != operators.end())
		operators.erase(oper);
}

void Channel::set_topic(const std::string& text) {
	topic.second = text;
}

const std::string& Channel::get_topic() const {
	return (topic.second);
}

void Channel::set_channel_limit(const bool state){
	invite_only.first = state;
	if (state == false)
		invite_only.second.clear();
}

void Channel::add_invite_user(const int user){
	invite_only.second.push_back(user);
	//std::cout << "current users: " << std::endl;
	for (auto i : invite_only.second)
		std::cout << i << std::endl;
}

bool Channel::channel_has_topic() {
	if (topic.second.empty())
		return (false);
	return (true);
}

void Channel::set_channel_topic_state(const bool state){
		topic.first = state;
}

bool Channel::is_topic_protected(void){
	return topic.first;
}

bool Channel::is_invite_only(void){
	return invite_only.first;
}

bool Channel::in_invite_list(const int user){
	for (auto i : invite_only.second){
		if (i == user)
			return true;
	}
	return false;
}