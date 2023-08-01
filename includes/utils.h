#ifndef UTILS_H
# define UTILS_H

#include <vector>
# include <string>

bool isEmpty(std::string const & str);
std::vector<std::string> split(std::string const & str, char delimiter);
void trim(std::string & str);

#endif