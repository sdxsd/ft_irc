#include "lib/utils.hpp"

std::string trimWhitespace(std::string &in) {
	int start = 0;
	int end = in.size();
	const char* ws = " \t\n\r\f\v";

	start = in.find_first_not_of(ws);
	end = (in.find_last_not_of(ws) + 1);
	return(in.substr(start, (end - start)));
}

std::vector<std::string> *split(const std::string& str, const std::string& delim) {
	std::vector<std::string> *strings = new std::vector<std::string>;
	size_t start;
	size_t end = 0;
	while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
		end = str.find(delim, start);
		strings->push_back(str.substr(start, end - start));
	}
	std::string& last = strings->back();
	size_t pos = last.find('\r');
	if (pos != std::string::npos)
		last[pos] = '\0';
	else if ((pos = last.find('\n')) != std::string::npos)
		last[pos] = '\0';
	return strings;
}

std::string validate_modestring(const std::string& modestring) {
	std::string invalid_chars = "";
	static const std::string valid_modes = "itkol";
	if (modestring[0] != '-' || modestring[0] != '+')
		invalid_chars += modestring[0];
	for (unsigned long i = 1; i < modestring.size(); i++)
		if (valid_modes.find(modestring[i]) == std::string::npos)
			invalid_chars += modestring[i];
	return (invalid_chars);
}
