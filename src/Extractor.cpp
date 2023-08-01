#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cctype>
#include <map>
#include "Extractor.hpp"
#include "utils.h"

std::string Extractor::str(std::string & text, std::string const & property) {
  std::string property_value = "";
  std::string property_name = property + "=";
  std::size_t found = text.find(property_name);
  if (found == std::string::npos) {
    throw std::runtime_error("Property '" + property + "' not found");
  }

  std::size_t start = found + property_name.length();
  std::size_t end = text.find(";", start);
  if (end == std::string::npos) {
    throw std::runtime_error("Property '" + property + "' value not found");
  }

  property_value = text.substr(start, end - start);
  if (isEmpty(property_value)) {
    throw std::runtime_error("Property '" + property + "' value is empty");
  }

  text = text.substr(0, found) + text.substr(end + 1, text.length() - end);
  return property_value;
}

bool Extractor::b(std::string & text, std::string const & property) {
  std::string property_value = Extractor::str(text, property);
  if (property_value == "true") {
    return true;
  } else if (property_value == "false") {
    return false;
  } else {
    throw std::runtime_error("Property '" + property + "' value is not a boolean");
  }
}

int Extractor::i(std::string & text, std::string const & property) {
  std::string property_value = Extractor::str(text, property);

  if (property_value.find_first_not_of("0123456789") != std::string::npos) {
    throw std::runtime_error("Property '" + property + "' value is not a number");
  }

  std::stringstream ss(property_value);
  int value;
  ss >> value;
  return value;
}

unsigned int Extractor::ui(std::string & text, std::string const & property) {
  std::string property_value = Extractor::str(text, property);

  if (property_value.find_first_not_of("0123456789") != std::string::npos) {
    throw std::runtime_error("Property '" + property + "' value is not a number");
  }

  std::stringstream ss(property_value);
  unsigned int value;
  ss >> value;
  return value;
}

std::vector<std::string> Extractor::v_str(std::string & text, std::string const & property) {
  std::string property_value = Extractor::str(text, property);
  std::vector<std::string> values = split(property_value, ',');
  return values;
}

std::vector<int> Extractor::v_i(std::string & text, std::string const & property) {
  std::string property_value = Extractor::str(text, property);
  std::vector<std::string> values = split(property_value, ',');
  std::vector<int> numbers;
  for (std::vector<std::string>::iterator it = values.begin(); it != values.end(); it++) {
    std::stringstream ss(*it);
    int value;
    ss >> value;

    numbers.push_back(value);
  }
  return numbers;
}

std::map<std::string, std::string> Extractor::m_str_str(std::string & text, std::string const & property) {
  std::string property_value = "";
  std::string property_name = property + "=";
  std::size_t found = text.find(property_name);
  if (found == std::string::npos) {
    throw std::runtime_error("Property '" + property + "' not found");
  }

  std::size_t start = text.find_first_not_of(" \t\r\n\f\v", found + property_name.length());
  if (start == std::string::npos || text[start] != '{') {
    throw std::runtime_error("Property '" + property + "' value not found");
  }

  start += 1;

  std::size_t current_position = start;
  std::size_t end = 0;
  std::size_t inner_start;
  do {
    inner_start = text.find("{", current_position);
    if (inner_start != std::string::npos) {
      current_position = text.find("}", inner_start);
    } else {
      end = text.find("}", current_position);
    }
  } while (current_position < text.length() && current_position != std::string::npos && end == 0);
  if (end == 0) {
    throw std::runtime_error("Property '" + property + "' value not found");
  }

  property_value = text.substr(start, end - start);
  trim(property_value);
  if (isEmpty(property_value)) {
    throw std::runtime_error("Property '" + property + "' value is empty");
  }

  std::vector<std::string> pairs = split(property_value, ';');
  std::map<std::string, std::string> map;
  for (std::vector<std::string>::iterator it = pairs.begin(); it != pairs.end(); it++) {
    trim(*it);
    std::vector<std::string> pair = split(*it, '=');
    if (pair.size() != 2) {
      throw std::runtime_error("Property '" + property + "' value is not a map");
    }
    map[pair[0]] = pair[1];
  }

  text = text.substr(0, found) + text.substr(end + 1, text.length() - end);
  return map;
}

std::map<int, std::string> Extractor::m_i_str(std::string & text, std::string const & property) {
  std::map<std::string, std::string> str_map = Extractor::m_str_str(text, property);
  std::map<int, std::string> int_map;

  for (std::map<std::string, std::string>::iterator it = str_map.begin(); it != str_map.end(); it++) {
    std::stringstream ss(it->first);
    int key;
    ss >> key;
    int_map[key] = it->second;
  }

  return int_map;
}