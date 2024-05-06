#include <string>
#include <map>
#include <functional>

std::map<std::string, std::function<std::string(std::string)>> init_message_db(void) {
	std::map<std::string, std::function<std::string(std::string)>> messages = {
		{
			"NICK",
			[](std::string arg) -> std::string {
				return ("001 " + arg + " Welcome to the Server!\r\n");
			}
		},
		{

		}
	};
}
