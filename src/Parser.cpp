#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cctype>
#include "Parser.hpp"
#include "Extractor.hpp"
#include "utils.h"

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

  std::string root = Extractor::str(locationChunk, "root");
  std::string route = Extractor::str(locationChunk, "route");
  std::string redirection = Extractor::str(locationChunk, "redirection");
  std::string defaultFile = Extractor::str(locationChunk, "default_file");

  std::vector<std::string> methods = Extractor::v_str(locationChunk, "methods");

  bool upload = Extractor::b(locationChunk, "upload");
  bool directoryListing = Extractor::b(locationChunk, "directory_listing");

  // TODO: cgiFileExtensionMap
  std::map<std::string, std::string> cgiFileExtensionMap;

  HttpLocation *location = new HttpLocation(root, route, methods, defaultFile, redirection, cgiFileExtensionMap, directoryListing, upload);

  return location;
}

HttpServer * Parser::extractServer() {
  std::string serverChunk = this->extractChunk(this->fileContent, "server");
  if (serverChunk.length() == 0) {
    return NULL;
  }

  std::vector<std::string> serverNames = Extractor::v_str(serverChunk, "server_names");

  unsigned int maxBody = Extractor::ui(serverChunk, "body_max");

  std::vector<int>ports = Extractor::v_i(serverChunk, "ports");

  // TODO: errorNumberToLocation
  std::map<int, std::string> errorNumberToLocation;

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
  if (!file) {
    throw std::runtime_error("Could not open configuration file '" + this->filePath + "'");
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  this->fileContent = buffer.str();
}
