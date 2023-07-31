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

    // TODO: Check in parser that root cannot be empty
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
			response = handle_get(request, location);
		} else if (request.getMethod() == "POST") {
			// response = handle_post(request, location);
		} else if (request.getMethod() == "DELETE") {
			response = handle_delete(request, location);
		} else if (request.getMethod() == "PUT") {
			// response = handle_put(request, location);
		} else response.setStatusMessage(RESPONSE_CODE__METHOD_NOT_ALLOWED);
    }

    response.addHeader("Connection", "close");
    response.addHeader("Server", this->serverNames[0]);
    // TODO: error pages

    return response;
}

HttpResponse HttpServer::handle_get(HttpRequest & request, HttpLocation *location) {
    HttpResponse response;
    std::string path = location->getRoot() + request.getRoute();

    if (isDirectory(path)) {
        std::string index = path + "/index.html";
        std::ifstream file(index.c_str());
        if (file.good()) {
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            response.setStatusMessage(RESPONSE_CODE__OK);
            response.setResponse(content);
        }
        else {
            // std::cout << "No index.html" << std::endl;
            if (location->directoryListingAllowed()) {
                // std::cout << "Directory listing allowed" << std::endl;
                std::string content = listDirectory(path, request.getRoute());
                response.setStatusMessage(RESPONSE_CODE__OK);
                response.setResponse(content);
            }
            else {
                // std::cout << "Directory listing not allowed" << std::endl;
                response.setStatusMessage(RESPONSE_CODE__NOT_FOUND);
                response.setResponse("File not found");
            }
        }
    } else {
        std::ifstream file(path.c_str());
        if (!file.good()) {
            response.setStatusMessage(RESPONSE_CODE__NOT_FOUND);
            response.setResponse("File not found");
            return response;
        }

        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        // TODO: CGIs

        response.setStatusMessage(RESPONSE_CODE__OK);
        response.setResponse(content);
    }
    return response;
}

HttpResponse HttpServer::handle_delete(HttpRequest & request, HttpLocation *location) {
    HttpResponse response;
    std::string path = location->getRoot() + request.getRoute();

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
    int locationRouteLength;

    location = NULL;
    for (std::vector<HttpLocation *>::iterator it = this->locations.begin(); it != this->locations.end() && location == NULL; it++) {
        locationRoute = (*it)->getRoute();
        locationRouteLength = locationRoute.size();

        if (locationRoute[locationRouteLength - 1] == '/') {
            locationRoute = locationRoute.substr(0, locationRouteLength - 1);
            locationRouteLength--;
        }
    
        subStringRoute = route.substr(0, locationRouteLength);

        if (locationRoute.compare(subStringRoute) == 0 && location == NULL) location = *it;
    }

    return location;
}

bool HttpServer::isDirectory(const std::string & path) {
    struct stat path_stat;
    return (stat(path.c_str(), &path_stat) == 0 && (path_stat.st_mode & S_IFDIR));
}

std::string HttpServer::listDirectory(const std::string & path, const std::string & requestRoute) {
    std::string content;
    DIR *dir;
    struct dirent *ent;
    std::string route;
    std::string link;
    std::string name;
    std::string size;
    std::string date;
    std::string type;
    std::string line;
    std::stringstream ss;
    std::string linkRoute;

    if ((dir = opendir(path.c_str())) != NULL) {
        content = "<!DOCTYPE html>\n<html>\n<head>\n<title>Index of " + path + "</title>\n</head>\n<body>\n<h1>Index of " + path + "</h1>\n<table>\n<tr><th>Link</th><th>Name</th><th>Last modified</th><th>Size</th><th>Description</th></tr>\n";
        while ((ent = readdir(dir)) != NULL) {
            ss.str(std::string());
            route = path + "/" + ent->d_name;
            linkRoute = requestRoute + "/" + std::string(ent->d_name);
            if (ent->d_type == DT_DIR) {
                type = "directory";
                if (std::string(ent->d_name).compare(".") == 0) {
                    linkRoute = requestRoute;
                    link = "<a href=\"" + linkRoute + "\">" + std::string(ent->d_name) + "/</a>";
                    name = std::string(ent->d_name) + "/";
                } else if (std::string(ent->d_name).compare("..") == 0) {
                    linkRoute = requestRoute.substr(0, requestRoute.find_last_of("/"));
                    linkRoute = requestRoute.substr(0, linkRoute.find_last_of("/"));
                    link = "<a href=\"" + linkRoute + "\">" + std::string(ent->d_name) + "/</a>";
                    name = std::string(ent->d_name) + "/";
                } else {
                    link = "<a href=\"" + linkRoute + "/\">" + std::string(ent->d_name) + "/</a>";
                    name = std::string(ent->d_name) + "/";
                }
            }
            else {
                link = "<a href=\"" + linkRoute + "\">" + std::string(ent->d_name) + "</a>";
                name = std::string(ent->d_name);
                type = "file";
            }
            struct stat path_stat;
            stat(route.c_str(), &path_stat);
            ss << path_stat.st_size;
            size = ss.str();
            date = std::string(ctime(&path_stat.st_mtime));
            date = date.substr(0, date.size() - 1);
            line = "<tr><td>" + link + "</td><td>" + name + "</td><td>" + date + "</td><td>" + size + "</td><td>" + type + "</td></tr>";
            content += line;
        }
        content += "</table>\n</body>\n</html>";
        closedir(dir);
    }
    else {
        std::cout << "Error opening directory" << std::endl;
    }

    return content;
}
