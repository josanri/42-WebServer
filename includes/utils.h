#ifndef UTILS_H
# define UTILS_H

#include <vector>
# include <string>

bool isEmpty(std::string const & str);
std::vector<std::string> split(std::string const & str, char delimiter);
std::vector<std::string> split(std::string const & str, std::string delimiter);
void trim(std::string & str);
void replaceAll(std::string & str, std::string const & s1, std::string const & s2);

#endif