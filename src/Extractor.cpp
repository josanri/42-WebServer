#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cctype>
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

