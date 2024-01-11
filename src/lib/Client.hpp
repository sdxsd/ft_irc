#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

#define NICKNAME_LENGTH_MAX 9

class Client {
	private:
		int					fd;
		const std::string	_username;
		const std::string	_nickname;
		const std::string	_hostname;
		const std::string	_realname;
};

#endif // CLIENT_HPP
