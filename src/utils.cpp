#include "lib/utils.hpp"

std::vector<std::string> split_by_delim(std::string str, char delim) {
	std::vector<std::string> strings;
	size_t start;
	size_t end = 0;
	while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
		end = str.find(delim, start);
		strings.push_back(str.substr(start, end - start));
	}
	return strings;
}
