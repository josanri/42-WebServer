#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <vector>

#include "HttpServer.hpp"
#include "ErrorCode.hpp"

HttpServer::HttpServer(void) {
	this->ports.push_back(8080);
    this->serverNames.push_back("localhost");
}

HttpServer::HttpServer(std::vector<int> & ports, std::vector<std::string> &  serverNames, std::vector<HttpLocation *>  & locations,
    std::map<int, std::string> & errorNumberToLocation, unsigned int maxBody) {
    this->ports = ports;
    this->locations = locations;
    this->serverNames = serverNames;
    this->errorNumberToLocation = errorNumberToLocation;
    this->maxBody = maxBody;
 }

HttpServer::~HttpServer() {
    
}

HttpServer & HttpServer::operator=(const HttpServer & rhs) {
    if (this != &rhs) {
        this->ports = rhs.ports;
        this->serverNames = rhs.serverNames;
        this->locations = rhs.locations;
        this->errorNumberToLocation = rhs.errorNumberToLocation;
        this->maxBody = rhs.maxBody;
    }
    return (*this);
}

HttpResponse HttpServer::processHttpRequest(HttpRequest & request)
{
    HttpResponse response;
    std::cout << "Processing request " << request.getMethod() << std::endl;

    HttpLocation *location = getLocation(request);
    if (location == NULL) {
        response.setStatusMessage(RESPONSE_CODE__NOT_FOUND);
        response.setResponse("File not found");
    }
    else if (!location->isMethodAllowed(request.getMethod())) {
        response.setStatusMessage(RESPONSE_CODE__METHOD_NOT_ALLOWED);
        response.setResponse("Method not allowed");
    }
    // TODO: Control max size
    else {
        std::cout << "adios" << std::endl;
        if (request.getMethod() == "GET") {
			// response = handle_get(request, location);
		} else if (request.getMethod() == "POST") {
			// response = handle_post(request, location);
		} else if (request.getMethod() == "DELETE") {
			response = handle_delete(request);
		} else if (request.getMethod() == "PUT") {
			// response = handle_put(request, location);
		} else response.setStatusMessage(RESPONSE_CODE__METHOD_NOT_ALLOWED);
    }

    response.addHeader("Connection", "close");
    response.addHeader("Server", this->serverNames[0]);
    // TODO: error pages

    return response;
}


HttpResponse HttpServer::handle_delete(HttpRequest & request) {
    HttpResponse response;
    std::string path = '.' + request.getRoute();

    if (path[path.length() - 1] == '/') path = path.substr(0, path.length() - 1);

    if (remove(path.c_str()) == 0) {
        response.setStatusMessage(RESPONSE_CODE__OK);
        response.setResponse("File deleted");
    }
    else {
        response.setStatusMessage(RESPONSE_CODE__NOT_FOUND);
        response.setResponse("File not found");
    }

    return response;
}

const std::vector<int> & HttpServer::getPorts() const {
	return (this->ports);
}

const std::vector<std::string> & HttpServer::getServerNames() const {
    return (this->serverNames);
}

std::vector<HttpLocation *> HttpServer::getLocations() {
    return (this->locations);
}


HttpLocation* HttpServer::getLocation(HttpRequest & request) {
    std::string route = request.getRoute();
    std::string subStringRoute;
    HttpLocation *location;
    std::string locationRoute;

    location = NULL;
    for (std::vector<HttpLocation *>::iterator it = this->locations.begin(); it != this->locations.end() && location == NULL; it++) {
        locationRoute = (*it)->getRoute();
    
        if (locationRoute[0] == '.') locationRoute = locationRoute.substr(1);
        if (locationRoute[locationRoute.length() - 1] == '/') locationRoute = locationRoute.substr(0, locationRoute.length() - 1);
        subStringRoute = route.substr(0, locationRoute.length());

        if (locationRoute.compare(subStringRoute) == 0 && location == NULL) location = *it;
    }

    return location;
}
