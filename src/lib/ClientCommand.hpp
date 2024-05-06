#ifndef CLIENTCOMMAND_HPP
#define CLIENTCOMMAND_HPP

#include <string>
#include <vector>
#include "Client.hpp"

class ClientCommand {
	private:
		Client&						sender;
		std::vector<std::string>	parameters;
		int 						parameter_count;
	public:
		const std::string name;
		ClientCommand(Client& sender, const std::string& name, std::string params);
};

#endif // CLIENTCOMMAND_HPP
