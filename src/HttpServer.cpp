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

HttpResponse HttpServer::processHttpRequest(HttpRequest & request)
{
    HttpResponse response;
    std::cout << "Processing request" << std::endl;

    HttpLocation *location = getLocation(request);
    if (location == NULL) response.setStatusCode(404);
    else if (!location->isMethodAllowed(request.getMethod())) response.setStatusCode(405);
    // TODO: Control max size
    else {
        if (request.getMethod() == "GET") {
			// response = handle_get(request, loc);
		} else if (request.getMethod() == "POST") {
			// response = handle_post(request, loc);
		} else if (request.getMethod() == "DELETE") {
			// response = handle_delete(request, loc);
		} else if (request.getMethod() == "PUT") {
			// response = handle_put(request, loc);
		} else response.setStatusCode(405);
    }

    response.addHeader("Connection", "close");
    // TODO: server name to headers

    return response;
}


const std::vector<int> & HttpServer::getPorts() const {
	return (this->ports);
}

const std::vector<std::string> & HttpServer::getServerNames() const {
    return (this->serverNames);
}

HttpLocation* HttpServer::getLocation(HttpRequest & request) {
    std::string route = request.getRoute();
    std::string subStringRoute;
    HttpLocation *location;
    std::string locationRoute;

    location = NULL;
    // TODO: Segfault here
    for (std::vector<HttpLocation *>::iterator it = this->locations.begin(); it != this->locations.end() && location == NULL; it++) {
        locationRoute = (*it)->getRoute();

        if (locationRoute[locationRoute.length() - 1] == '/') locationRoute = locationRoute.substr(0, locationRoute.length() - 1);
        subStringRoute = route.substr(0, locationRoute.length());

        if (locationRoute.compare(subStringRoute) == 0 && location == NULL) location = *it;
    }

    return location;
}
