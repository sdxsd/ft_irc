#include "lib/utils.hpp"

std::string trimWhitespace(std::string &in) {
	int start = 0;
	int end = in.size();
	const char* ws = " \t\n\r\f\v";

	start = in.find_first_not_of(ws);
	end = (in.find_last_not_of(ws) + 1);
	return(in.substr(start, (end - start)));
}

std::vector<std::string> split(const std::string& str, char delim) {
	std::vector<std::string> strings;
	size_t start;
	size_t end = 0;
	while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
		end = str.find(delim, start);
		strings.push_back(str.substr(start, end - start));
	}
	std::string& last = strings.back();
	size_t pos = last.find('\r');
	if (pos != std::string::npos)
		last[pos] = '\0';
	else if ((pos = last.find('\n')) != std::string::npos)
		last[pos] = '\0';
	return strings;
}
