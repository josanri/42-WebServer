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
#include <algorithm>

#include "HttpLocation.hpp"

HttpLocation::HttpLocation(void) {
    this->directoryListing = false;
	this->upload = false;
}

HttpLocation::HttpLocation(std::string route, std::vector<std::string> & methods, std::string defaultFile, std::string redirectionRoute, std::map<std::string, std::string> & fileExtensionToCGI, bool directoryListing, bool upload) {
    this->route = route;
    this->methods = methods;
    this->defaultFile = defaultFile;
    this->redirectionRoute = redirectionRoute;
    this->fileExtensionToCGI = fileExtensionToCGI;
    this->directoryListing = directoryListing;
	this->upload = upload;
}

HttpLocation::~HttpLocation() {
    
}

std::string HttpLocation::getRoute() const {
    return (this->route);
}

bool HttpLocation::isMethodAllowed(std::string method) {
    std::vector<std::string>::iterator it = std::find(this->methods.begin(), this->methods.end(), method);

    return it != this->methods.end();
}
