#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <vector>

#include "HttpServer.hpp"
#include "ErrorCode.hpp"

#include "utils.h"

HttpServer::HttpServer(void) {
	this->ports.push_back(8080);
    this->serverNames.push_back("localhost");
}

HttpServer::HttpServer(std::vector<int> & ports, std::vector<std::string> &  serverNames, std::vector<HttpLocation *>  & locations,
    std::map<int, std::string> & errorNumberToLocation) {
    this->ports = ports;
    this->locations = locations;
    this->serverNames = serverNames;
    this->errorNumberToLocation = errorNumberToLocation;
 }

HttpServer::~HttpServer() {
    
}

HttpServer & HttpServer::operator=(const HttpServer & rhs) {
    if (this != &rhs) {
        this->ports = rhs.ports;
        this->serverNames = rhs.serverNames;
        this->locations = rhs.locations;
        this->errorNumberToLocation = rhs.errorNumberToLocation;
    }
    return (*this);
}

HttpResponse HttpServer::processHttpRequest(HttpRequest & request)
{
    HttpResponse response;
    HttpLocation *location = getLocation(request);
    
    std::cout << "Length ?" << request.getContentLength() << std::endl;
    if (location != NULL && location->getRedirectionRoute() != "" && location->getRedirectionRoute() != request.getRoute()) {
        response.setStatusMessage(RESPONSE_CODE__FOUND);
        response.addHeader("Location", location->getRedirectionRoute());
        response.setResponse("");
    } else if (location == NULL) {
        response.setStatusMessage(RESPONSE_CODE__NOT_FOUND);
        response.setResponse("File not found");
    }
    else if (!location->isMethodAllowed(request.getMethod())) {
        response.setStatusMessage(RESPONSE_CODE__METHOD_NOT_ALLOWED);
        response.setResponse("Method not allowed");
    }
    else if (request.getContentLength() > location->getMaxBodySize()) {
        response.setStatusMessage(RESPONSE_CODE__PAYLOAD_TOO_LARGE);
        response.setResponse("Payload too large");
    }
    else {
        size_t locationRouteLength = location->getRoute().size();

        if (location->getRoute()[locationRouteLength - 1] == '/') {
            locationRouteLength--;
        }
        request.setRoute(request.getRoute().substr(locationRouteLength));
    
        if (request.getMethod() == "GET") {
			response = handle_get(request, location);
		} else if (request.getMethod() == "POST") {
			response = handle_post(request, location);
		} else if (request.getMethod() == "DELETE") {
			response = handle_delete(request, location);
		} else if (request.getMethod() == "PUT") {
			response = handle_put(request, location);
		} else response.setStatusMessage(RESPONSE_CODE__METHOD_NOT_ALLOWED);
    }

    response.addHeader("Connection", "close");
    response.addHeader("Server", this->serverNames[0]);
    return response;
}

HttpResponse HttpServer::handle_get(HttpRequest & request, HttpLocation *location) {
    HttpResponse response;
    std::string path = location->getRoot() + request.getRoute();

    if (isDirectory(path)) {
        std::string index = path + "/" + location->getDefaultFile();
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
        
        std::string extension = path.substr(path.find_last_of(".") + 1);
        std::string cgiPath = location->getCgi(extension);
        if (isEmpty(extension) || isEmpty(cgiPath)) {
            response.setStatusMessage(RESPONSE_CODE__OK);
            response.setResponse(content);
            return response;
        }

        response = cgi(request, path, cgiPath);
    }
    return response;
}

HttpResponse HttpServer::handle_post(HttpRequest & request, HttpLocation *location) {
    HttpResponse response;
    std::string path = location->getRoot() + request.getRoute();
    std::string extension = path.substr(path.find_last_of(".") + 1);

    if (isEmpty(extension)) {
        response.setStatusMessage(RESPONSE_CODE__BAD_REQUEST);
        response.setResponse("No extension");
        return response;
    }

    std::string cgiPath = location->getCgi(extension);
    if (isEmpty(cgiPath)) {
        response.setStatusMessage(RESPONSE_CODE__BAD_REQUEST);
        response.setResponse("No CGI for extension");
        return response;
    }
    return cgi(request, path, cgiPath);
}

HttpResponse HttpServer::handle_put(HttpRequest & request, HttpLocation *location) {
    HttpResponse response;

    if (!location->getUpload()) {
        response.setStatusMessage(RESPONSE_CODE__FORBIDDEN);
        return response;
    }

    std::string path = location->getRoot() + request.getRoute();

    if (isDirectory(path)) {
        response.setStatusMessage(RESPONSE_CODE__BAD_REQUEST);
        response.setResponse("Route is a directory");

        return response;
    }

    std::vector<std::string> folders = split(path, '/');

    std::string folder = "";
    for (std::vector<std::string>::iterator it = folders.begin(); it != folders.end() - 1; it++) {
        folder += *it + "/";
        if (!exists(folder)) {
            if (mkdir(folder.c_str(), 0777) == -1) {
                response.setStatusMessage(RESPONSE_CODE__INTERNAL_SERVER_ERROR);
                response.setResponse("Error creating folder");
                return response;
            }
        }
        else if (!isDirectory(folder)) {
            response.setStatusMessage(RESPONSE_CODE__BAD_REQUEST);
            response.setResponse("Route is a file");
            return response;
        }
    }

    std::ofstream file(path.c_str());
    if (!file.good()) {
        response.setStatusMessage(RESPONSE_CODE__INTERNAL_SERVER_ERROR);
        response.setResponse("Error creating file");
        return response;
    }

    file << request.getBody();
    file.close();

    response.setStatusMessage(RESPONSE_CODE__CREATED);
    response.setResponse("File created");

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

const std::map<int, std::string> & HttpServer::getErrorNumberToLocation() const {
    return (this->errorNumberToLocation);
}


HttpLocation* HttpServer::getLocation(HttpRequest & request) {
    std::string route = request.getRoute();
    std::string subStringRoute;
    HttpLocation *location;
    std::string locationRoute;
    int locationRouteLength;

    location = NULL;
    for (std::vector<HttpLocation *>::iterator it = this->locations.begin(); it != this->locations.end(); it++) {
        locationRoute = (*it)->getRoute();
        locationRouteLength = locationRoute.size();

        if (locationRoute[locationRouteLength - 1] == '/') {
            locationRoute = locationRoute.substr(0, locationRouteLength - 1);
            locationRouteLength--;
        }

        int firstSlashAfterLocationRoute = route.find('/', locationRouteLength);
        subStringRoute = route.substr(0, firstSlashAfterLocationRoute);

        bool isMethodAllowed = (*it)->isMethodAllowed(request.getMethod());

        if (locationRoute.compare(subStringRoute) == 0 && (location == NULL || isMethodAllowed)) location = *it;
    }

    return location;
}

bool HttpServer::exists(const std::string & path) {
    struct stat path_stat;
    return (stat(path.c_str(), &path_stat) == 0);
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
    std::string name;
    std::string size;
    std::string date;
    std::string type;
    std::string line;
    std::stringstream ss;
    std::string linkRoute;

    if ((dir = opendir(path.c_str())) != NULL) {
        content = "<!DOCTYPE html>\n<html>\n<head>\n<title>Index of " + path + "</title>\n</head>\n<body>\n<h1>Index of " + path + "</h1>\n<table>\n<tr><th>Name</th><th>Last modified</th><th>Size</th><th>Description</th></tr>\n";
        while ((ent = readdir(dir)) != NULL) {
            ss.str(std::string());
            route = path;
            if (route[route.size() - 1] != '/') route += "/";
            route += ent->d_name;
            linkRoute = requestRoute + std::string(ent->d_name);
            if (ent->d_type == DT_DIR) {
                type = "directory";
                if (std::string(ent->d_name).compare(".") == 0) {
                    name = std::string(ent->d_name) + "/";
                } else if (std::string(ent->d_name).compare("..") == 0) {
                    name = std::string(ent->d_name) + "/";
                } else {
                    name = std::string(ent->d_name) + "/";
                }
            }
            else {
                name = std::string(ent->d_name);
                type = "file";
            }
            struct stat path_stat;
            stat(route.c_str(), &path_stat);
            ss << path_stat.st_size;
            size = ss.str();
            date = std::string(ctime(&path_stat.st_mtime));
            date = date.substr(0, date.size() - 1);
            line = "<tr><td>" + name + "</td><td>" + date + "</td><td>" + size + "</td><td>" + type + "</td></tr>";
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

HttpResponse HttpServer::cgi (HttpRequest & request, std::string & path, std::string & cgiPath) {
    HttpResponse response;
    std::string body = request.getBody();

    if (!exists(cgiPath)) {
        response.setStatusMessage(RESPONSE_CODE__NOT_FOUND);
        response.setResponse("CGI not found");
        return response;
    }

    std::ofstream file("cgiInput");
    file << body;
    file.close();
    int fdin = open("cgiInput", O_RDONLY);
    int fdout = open("cgiOutput", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    pid_t pid = fork();
    if (pid == -1) {
        response.setStatusMessage(RESPONSE_CODE__INTERNAL_SERVER_ERROR);
        response.setResponse("Error forking");
        return response;
    }

    std::vector<std::string> args = split(cgiPath, ' ');
    std::string cgi = args[0];

    char *argv[args.size() + 2];
    argv[args.size()] = (char *)path.c_str();
    argv[args.size() + 1] = NULL;
    for (unsigned int i = 0; i < args.size(); i++) {
        argv[i] = (char *)args[i].c_str();
    }

    int bodySize = body.size();

    if (pid == 0) {
        dup2(fdin, STDIN_FILENO);
        dup2(fdout, STDOUT_FILENO);

        std::stringstream ss;
        ss << bodySize;
        std::string contentLength = "CONTENT_LENGTH=" + ss.str();
        std::string pathInfo = "PATH_INFO=" + request.getRoute();
        std::string requestMethod = "REQUEST_METHOD=" + request.getMethod();
        char **envp = new char*[5 + request.getHeaders().size()];
        
        int i = 0;
        envp[i++] = (char *)contentLength.c_str();
        envp[i++] = (char *)pathInfo.c_str();
        envp[i++] = (char *)requestMethod.c_str();
        envp[i++] = (char *)"SERVER_PROTOCOL=HTTP/1.1";

        // Iterate over headers of request and add to envp
        std::map<std::string, std::string> headers = request.getHeaders();
        for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it) {
            std::string header = it->first;
            std::string value = it->second;
            std::string env = header + "=" + value;
            std::string str = "HTTP_";
            envp[i++] = (char *)(str + env).c_str();
        }

        envp[i] = NULL;
        if (execve(cgi.c_str(), argv, envp) == -1) {
            std::cerr << "Error executing CGI" << std::endl;
            close(fdin);
            close(fdout);
            delete[] envp;
            exit(1);
        }

        exit(0);
    }

    int status;
    waitpid(pid, &status, 0);

    close(fdin);

    // If the child process exited with an error, return 500
    if (status != 0) {
        close(fdout);
        response.setStatusMessage(RESPONSE_CODE__INTERNAL_SERVER_ERROR);
        response.setResponse("Error executing CGI");
   
        return response;
    }

    std::string responseContent;
    std::stringstream ss;

    close(fdout);

    std::ifstream file2("cgiOutput");
    ss << file2.rdbuf();
    file2.close();

    responseContent = ss.str();


    size_t crlfcrlf = responseContent.find("\r\n\r\n");
    if (crlfcrlf == std::string::npos) {
        response.setResponse(responseContent);
        return response;
    }
    std::string headers = responseContent.substr(0, crlfcrlf);

    std::string bodyResponse = responseContent.substr(crlfcrlf + 4);
    response.setResponse(bodyResponse);

    std::vector<std::string> lines = split(headers, "\r\n");
    for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); it++) {
        std::cout << "Señales de vida" << *it << std::endl;
        std::vector<std::string> header = split(*it, ':');
        if (header.size() == 2) {
            response.addHeader(header[0], header[1]);
            std::cout << "COPIIII" << header[0] << header[1] << std::endl;
        }
    }

    std::remove("cgiInput");
    std::remove("cgiOutput");
    return response;
}
