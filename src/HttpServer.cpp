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

HttpServer::~HttpServer() {
    
}

HttpResponse HttpServer::processHttpRequest(HttpRequest & request)
{
    (void) request;
    return HttpResponse();
}


const std::vector<int> & HttpServer::getPorts() const {
	return (this->ports);
}