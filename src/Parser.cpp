#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include "Parser.hpp"

std::string extract_property(std::string & file_content, std::string const & property) {
  std::string property_value = "";
  std::string property_name = property + "=";
  std::size_t found = file_content.find(property_name);
  if (found != std::string::npos) {
    std::size_t start = found + property_name.length();
    std::size_t end = file_content.find(";", start);
    property_value = file_content.substr(start, end - start);
    file_content = file_content.substr(0, found) + file_content.substr(end + 1, file_content.length() - end);
  }
  return property_value;
}

int extract_number_property(std::string & file_content, std::string const & property) {
  std::string property_value = extract_property(file_content, property);
  std::stringstream ss(property_value);
  int value;
  ss >> value;
  return value;
}

Parser::Parser(std::string const & filePath): filePath(filePath) {

}

Parser::~Parser () {
}


// std::vector<HttpServer *> Parser::parse(void) {
void Parser::parse(std::vector<HttpServer *> servers) {
  this->fileContent = "server={ \
  ports=80,400; \
  server_names=www.google.com,www.google.es; \
  location={ \
    route=/; \
    methods=GET,POST; \
    redirection=/one; \
    default_file=index.html; \
    directory_search=/tmp/www; \
    directory_listing=false; \
    cgi_file_extension=.php,.alho; \
  } \
  error_pages=/error_pages; \
  body_max=500; \
}";

  HttpServer *server = this->extractServer();
  servers.push_back(server);

  // do {
  //   HttpServer *server = this->extractServer();
  //   if (server != NULL) {
  //     servers.push_back(server);
  //   } else {
  //     // Free memory
  //   }
  // } while (this->fileContent.length() > 0);


  // std::string location_chunk = extract_chunk_from_file(server_chunk, "location");
  // std::cout  << server_chunk << std::endl;
  // std::cout<<location_chunk<<std::endl;

  // std::string location_route = extract_property(location_chunk, "route");
  // std::string location_methods = extract_property(location_chunk, "methods");

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


HttpLocation * Parser::extractLocation(std::string & serverChunk) {
  std::string locationChunk = this->extractChunk(serverChunk, "location");
  if (locationChunk.length() == 0) {
    return NULL;
  }
  std::string route = extract_property(locationChunk, "route");
  std::string methods = extract_property(locationChunk, "methods");
  std::string redirection = extract_property(locationChunk, "redirection");
  std::string defaultFile = extract_property(locationChunk, "default_file");
  std::string directorySearch = extract_property(locationChunk, "directory_search");
  std::string directoryListing = extract_property(locationChunk, "directory_listing");
  // std::string cgiFileExtension = extract_property(locationChunk, "cgi_file_extension");

  std::vector<std::string> methodsVector = split(methods, ',');
  // std::vector<std::string>::iterator methodsIterator;
  // for (methodsIterator = methodsVector.begin(); methodsIterator != methodsVector.end(); methodsIterator++) {
  //   std::cout << *methodsIterator << std::endl;
  // }

  // std::vector<std::string> cgiFileExtensionVector = split(cgiFileExtension, ',');
  // std::vector<std::string>::iterator cgiFileExtensionIterator;
  // for (cgiFileExtensionIterator = cgiFileExtensionVector.begin(); cgiFileExtensionIterator != cgiFileExtensionVector.end(); cgiFileExtensionIterator++) {
  //   std::cout << *cgiFileExtensionIterator << std::endl;
  // }

  std::map<std::string, std::string> cgiFileExtensionMap;

  HttpLocation *location = new HttpLocation(route, methodsVector, defaultFile, redirection, cgiFileExtensionMap, false, false);

  return location;
}

HttpServer * Parser::extractServer() {
  std::string serverChunk = this->extractChunk(this->fileContent, "server");
  if (serverChunk.length() == 0) {
    return NULL;
  }
  std::cout << "ServerChunk: " << std::endl << serverChunk << std::endl;

  std::string serverNamesRAW = extract_property(serverChunk, "server_names");
  std::vector<std::string> serverNames = split(serverNamesRAW, ',');

  std::string portsRAW = extract_property(serverChunk, "ports");
  std::vector<std::string> portsVector = split(portsRAW, ',');
  std::vector<int>ports;

  std::vector<std::string>::iterator portsIterator;
  for (portsIterator = portsVector.begin(); portsIterator != portsVector.end(); portsIterator++) {
    std::stringstream ss(*portsIterator);
    int port;
    ss >> port;
    ports.push_back(port);
  }

  std::map<int, std::string> errorNumberToLocation;
  std::string errorPagesRAW = extract_property(serverChunk, "error_pages");
  
  unsigned int maxBody = extract_number_property(serverChunk, "body_max");

  std::cout << "ServerChunk: " << std::endl << serverChunk << std::endl;

  std::vector<HttpLocation *> locations;
  HttpLocation *location = this->extractLocation(serverChunk);
  do {
    locations.push_back(location);
    location = this->extractLocation(serverChunk);
    std::cout << "ServerChunk: " << std::endl << serverChunk << std::endl;
  } while (location != NULL);

  HttpServer *server = new HttpServer(ports, serverNames, locations, errorNumberToLocation, maxBody);

  return server;



  // int         body_max = extract_number_property(serverChunk, "body_max");


  // std::cout << "Parseruration:" << std::endl
  //           << "  error_pages: " << error_pages << std::endl
  //           << "  server_names: " << server_names << std::endl
  //           << "  ports: " << ports << std::endl
  //           << "  body_max: " << body_max << std::endl;
  //           // << "  location:" << std::endl
  //           //   << "    route: " << location_route << std::endl
  //           //   << "    methods: " << location_methods << std::endl;

  // return server;
}

std::string Parser::extractChunk(std::string & content, std::string const & chunk_name) {
  std::string chunk = "";
  std::string chunk_start = chunk_name + "={";
  std::string chunk_end = "}";

  std::size_t start = content.find(chunk_start);
  if (start != std::string::npos) {
    start += chunk_start.length();
    std::size_t end = content.find(chunk_end, start);
    if (end == std::string::npos) {
      return "";
    }
    chunk = content.substr(start, end - start);
    if (chunk.find("={") != std::string::npos) {
      end = content.find(chunk_end, end + 1);
      if (end == std::string::npos) {
        return "";
      }
      chunk = content.substr(start, end - start);
    }
    content = content.substr(start, content.length() - end);
  }

  return chunk;
}


void Parser::readFile () {
  std::ifstream file(this->filePath.c_str());
  std::stringstream buffer;

  if (file) {
    buffer << file.rdbuf();
    this->fileContent = buffer.str();
  } else {
    // TODO: throw ERR
    std::cerr << "Error: Could not open file " << this->filePath << std::endl;
  }
}
