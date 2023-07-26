#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cctype>
#include "Parser.hpp"
#include "utils.h"

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

void Parser::parse(std::vector<HttpPortListener *> & listeners, std::map<int,HttpPortListener *> & fileDescriptoToPort) {
  this->readFile();
  std::map<int,std::vector<HttpServer *> > portToServer;
  
  do {
    HttpServer *server = this->extractServer();
    if (server != NULL) {
      std::vector<int>::iterator portsIterator;
      std::vector<int> ports = server->getPorts();
      for (portsIterator = ports.begin(); portsIterator != ports.end(); portsIterator++) {
        int port = *portsIterator;

        std::map<int,std::vector<HttpServer *> >::iterator portToServerIterator = portToServer.find(port);

        if (portToServerIterator == portToServer.end()) {
          std::vector<HttpServer *> servers;
          servers.push_back(server);
          portToServer[port] = servers;
        } else {
          portToServerIterator->second.push_back(server);
        }
      }
    } else {
      // Free memory
      std::cerr << "Error: Could not parse server" << std::endl;
      return;
    }
  } while (!isEmpty(this->fileContent));

  std::map<int,std::vector<HttpServer *> >::iterator portToServerIterator;
  for (portToServerIterator = portToServer.begin(); portToServerIterator != portToServer.end(); portToServerIterator++) {
    int port = portToServerIterator->first;
    std::vector<HttpServer *> servers = portToServerIterator->second;
    HttpPortListener *listener = new HttpPortListener(port, fileDescriptoToPort, servers);
    listener->initializeSocket();
    listeners.push_back(listener);
  }
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
  std::string upload = extract_property(locationChunk, "upload");
  std::string directoryListing = extract_property(locationChunk, "directory_listing");

  // TODO: check bools
  bool directoryListingBool = directoryListing == "true";
  bool uploadBool = upload == "true";

  std::vector<std::string> methodsVector = split(methods, ',');

  // TODO: next line
  std::map<std::string, std::string> cgiFileExtensionMap;

  HttpLocation *location = new HttpLocation(route, methodsVector, defaultFile, redirection, cgiFileExtensionMap, directoryListingBool, uploadBool);

  return location;
}

HttpServer * Parser::extractServer() {
  std::string serverChunk = this->extractChunk(this->fileContent, "server");
  if (serverChunk.length() == 0) {
    return NULL;
  }

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

  // TODO: errorNumberToLocation
  std::map<int, std::string> errorNumberToLocation;
  std::string errorPagesRAW = extract_property(serverChunk, "error_pages");
  
  unsigned int maxBody = extract_number_property(serverChunk, "body_max");

  std::vector<HttpLocation *> locations;
  HttpLocation *location = this->extractLocation(serverChunk);
  do {
    locations.push_back(location);
    location = this->extractLocation(serverChunk);
  } while (location != NULL);

  HttpServer *server = new HttpServer(ports, serverNames, locations, errorNumberToLocation, maxBody);

  return server;
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

  // TODO: throw ERR if empty

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
