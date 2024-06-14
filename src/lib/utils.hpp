#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>

std::string validate_modestring(const std::string& modestring);
std::vector<std::string> *split(const std::string& str, const std::string& delim);
std::string trimWhitespace(std::string &in);

#endif // UTILS_HPP
