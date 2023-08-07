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

HttpLocation::HttpLocation(std::string root, std::string route, std::vector<std::string> & methods, std::string defaultFile, std::string redirectionRoute, std::map<std::string, std::string> & fileExtensionToCGI, bool directoryListing, bool upload, unsigned int maxBodySize) {
    this->root = root;
    this->route = route;
    this->methods = methods;
    this->defaultFile = defaultFile;
    this->redirectionRoute = redirectionRoute;
    this->fileExtensionToCGI = fileExtensionToCGI;
    this->directoryListing = directoryListing;
	this->upload = upload;
    this->maxBodySize = maxBodySize;
}

HttpLocation::~HttpLocation() {
    
}

std::string HttpLocation::getRoute() const {
    return (this->route);
}

std::string HttpLocation::getRoot() const {
    return (this->root);
}

std::string HttpLocation::getDefaultFile() const {
    return (this->defaultFile);
}

std::string HttpLocation::getRedirectionRoute() const {
    return (this->redirectionRoute);
}

std::string HttpLocation::getCgi(std::string extension) const {
    std::map<std::string, std::string>::const_iterator it = this->fileExtensionToCGI.find(extension);

    if (it == this->fileExtensionToCGI.end()) {
        return ("");
    }

    return (it->second);
}

bool HttpLocation::directoryListingAllowed() const {
    return (this->directoryListing);
}

bool HttpLocation::isMethodAllowed(std::string method) {
    std::vector<std::string>::iterator it = std::find(this->methods.begin(), this->methods.end(), method);

    return it != this->methods.end();
}

unsigned int HttpLocation::getMaxBodySize() const {
    return (this->maxBodySize);
}
