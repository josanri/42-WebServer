#include <string>
#include <vector>
#include <sstream>
#include "utils.h"

bool isEmpty(std::string const & str) {
  if (str.empty()) return (true);

  bool onlySpaces = str.find_first_not_of(" \t\n\v\f\r") == std::string::npos;
  return (onlySpaces);
}

std::vector<std::string> split(std::string const & str, char delimiter) {
  std::vector<std::string> result;
  std::stringstream ss(str);
  std::string token;
  while (std::getline(ss, token, delimiter)) {
    result.push_back(token);
  }
  return result;
}
