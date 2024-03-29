#include <unistd.h>
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
    if (!server) {
      for (std::map<int,std::vector<HttpServer *> >::iterator portToServerIterator = portToServer.begin(); portToServerIterator != portToServer.end(); portToServerIterator++) {
        std::vector<HttpServer *> servers = portToServerIterator->second;
        for (std::vector<HttpServer *>::iterator serversIterator = servers.begin(); serversIterator != servers.end(); serversIterator++) {
          std::vector<HttpLocation *> serverLocationVector = (*serversIterator)->getLocations();
          for (std::vector<HttpLocation *>::iterator locationIterator = serverLocationVector.begin(); locationIterator != serverLocationVector.end(); ++locationIterator)
          {
            delete *locationIterator;
          }
          delete *serversIterator;
        }
      }

      throw std::runtime_error("Error while parsing configuration file");
    }

    std::vector<int> ports = server->getPorts();
    for (std::vector<int>::iterator portsIterator = ports.begin(); portsIterator != ports.end(); portsIterator++) {
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
  } while (!isEmpty(this->fileContent));

  if (portToServer.size() == 0) {
    std::cerr << "Error: No server found" << std::endl;
    return;
  }

  std::map<int,std::vector<HttpServer *> >::iterator portToServerIterator;
  for (portToServerIterator = portToServer.begin(); portToServerIterator != portToServer.end(); portToServerIterator++) {
    int port = portToServerIterator->first;
    std::vector<HttpServer *> servers = portToServerIterator->second;
    HttpPortListener *listener = new HttpPortListener(port, fileDescriptoToPort, servers);
    listeners.push_back(listener);
    try {
      listener->initializeSocket();
    } catch (std::exception & e) {
      // TODO: Free memory
      freeMemory(listeners);
      throw std::runtime_error("Error while initializing sockets");
    }
  }
}


HttpLocation * Parser::extractLocation(std::string & serverChunk) {
  std::string locationChunk = this->extractChunk(serverChunk, "location");
  if (locationChunk.length() == 0) {
    return NULL;
  }

  std::string root = Extractor::str(locationChunk, "root");
  std::string route = Extractor::str(locationChunk, "route");
  std::string redirection;
  try{
     redirection = Extractor::str(locationChunk, "redirection");
  } catch (std::runtime_error & e) { 
    redirection = "";
  }
  std::string defaultFile = Extractor::str(locationChunk, "default_file");
  unsigned int maxBody = Extractor::ui(locationChunk, "body_max");

  std::vector<std::string> methods = Extractor::v_str(locationChunk, "methods");

  bool upload = Extractor::b(locationChunk, "upload");
  bool directoryListing = Extractor::b(locationChunk, "directory_listing");

  std::map<std::string, std::string> cgiFileExtensionMap = Extractor::m_str_str(locationChunk, "cgi");
  HttpLocation *location = new HttpLocation(root, route, methods, defaultFile, redirection, cgiFileExtensionMap, directoryListing, upload, maxBody);

  return location;
}

HttpServer * Parser::extractServer() {
  std::string serverChunk = this->extractChunk(this->fileContent, "server");
  if (serverChunk.length() == 0) {
    return NULL;
  }

  std::vector<std::string> serverNames = Extractor::v_str(serverChunk, "server_names");

  std::vector<int>ports = Extractor::v_i(serverChunk, "ports");

  std::map<int, std::string> errorNumberToLocation = Extractor::m_i_str(serverChunk, "error_pages");

  std::vector<HttpLocation *> locations;
  do {
     try {
        HttpLocation *location = this->extractLocation(serverChunk);
        if (!location) {
          throw std::runtime_error("Could not extract the location");
        } else {
          locations.push_back(location);
        }
      } catch (std::exception & e) {
        std::cout << e.what() << std::endl;
        for (std::vector<HttpLocation *>::iterator locationsIterator = locations.begin(); locationsIterator != locations.end(); locationsIterator++) {
            delete *locationsIterator;
          }
        return (NULL);
      }
  
  } while (!isEmpty(serverChunk));

  if (locations.size() == 0) {
    std::cerr << "Error: No location found" << std::endl;
    return NULL;
  }
  HttpServer *server = new HttpServer(ports, serverNames, locations, errorNumberToLocation);

  return server;
}

std::string Parser::extractChunk(std::string & content, std::string const & chunk_name) {
  std::string chunk_start = chunk_name + "={";
  std::string chunk_end = "}";

  std::size_t start = content.find(chunk_start);
  if (start == std::string::npos) {
    return "";
  }

  start += chunk_start.length();

  std::size_t current_position = start;
  std::size_t inner_start;
  std::size_t end;

  // Handle nested chunks. The inner chunk must be closed before the outer chunk. There can be multiple inner chunks in each inner and outer chunk.
  int opened_brackets = 1;
  do {
    inner_start = content.find("={", current_position);
    end = content.find("}", current_position);

    if (inner_start != std::string::npos && inner_start < end) {
      opened_brackets++;
      current_position = inner_start + 2;
    }
    if (end != std::string::npos && inner_start > end) {
      opened_brackets--;
      current_position = end + 1;
    }
  } while (opened_brackets > 0 && end != std::string::npos && current_position < content.length());

  if (opened_brackets > 0 || end == std::string::npos) {
    throw std::runtime_error("Chunk '" + chunk_name + "' not closed");
  }

  std::string chunk = content.substr(start, end - start);
  trim(chunk);

  std::string s1 = content.substr(0, start - chunk_start.length());
  std::string s2 = content.substr(end + chunk_end.length(), std::string::npos);

  content = s1 + s2;
  return chunk;
}


void Parser::readFile () {
  std::string file_extension = this->filePath.substr(this->filePath.find_last_of(".") + 1);
  if (file_extension != "cfg") {
    throw std::runtime_error("Configuration file must have the .cfg extension");
  }

  std::ifstream file(this->filePath.c_str());
  if (!file) {
    throw std::runtime_error("Could not open configuration file '" + this->filePath + "'");
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  this->fileContent = buffer.str();

  file.close();
}
