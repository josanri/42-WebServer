#include <string>
#include <vector>
#include <sstream>
#include "utils.h"

bool isEmpty(std::string const & str) {
  if (str.empty()) return (true);

  bool onlySpaces = str.find_first_not_of(" \t\n\v\f\r") == std::string::npos;
  return (onlySpaces);
}

void replaceAll(std::string & str, std::string const & s1, std::string const & s2)
{
    size_t pos = str.find(s1);
    while (pos != std::string::npos)
    {
        str.erase(pos, s1.length());
        str.insert(pos, s2);
        pos = str.find(s1, pos + s2.length());
    }
}

std::vector<std::string> split(std::string const & str, std::string delimiter) {
  std::vector<std::string> result;
  std::string token;

  size_t pos_1 = 0;
  size_t pos_2 = 0;
  while ((pos_2 = str.find(delimiter, pos_1)) != std::string::npos)
  {
      token = str.substr(pos_1, pos_2);
      result.push_back(token);
      pos_1 = pos_2 + token.length();
  }
  if (pos_1 == 0 && pos_2 == 0)
  {
    result.push_back(str);
  }
  return result;
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

void trim(std::string & str) {
  std::size_t start = str.find_first_not_of(" \t\r\n\f\v");
  if (start == std::string::npos) {
    str = "";
    return;
  }

  std::size_t end = str.find_last_not_of(" \t\r\n\f\v");
  str = str.substr(start, end - start + 1);
}