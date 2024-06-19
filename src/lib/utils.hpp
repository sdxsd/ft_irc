#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>

std::string validate_modestring(const std::string& modestring);
std::vector<std::string> *split(const std::string& str, const std::string& delim);
std::string trimWhitespace(std::string &in);
int count_delimiters(const std::string& str, const std::string& word);
std::string get_delimiter(const std::string& command);
bool last_contains_delimiter(const std::string& str);

#endif // UTILS_HPP
