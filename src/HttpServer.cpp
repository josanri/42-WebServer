#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include "HttpServer.hpp"
#include "HttpServerConfiguration.hpp"


// HttpServer::HttpServer(const HttpServer & src)
// {
// 	*this = src;
// }

// void doDelete(std::string & pathname) {
// 	if (remove(pathname.c_str()) != 0) {
// 		std::cerr << __func__ << ":" << __LINE__ << " error at deleting the file " << pathname << std::endl;
// 	}
// }

// HttpServer & HttpServer::operator=(HttpServer const & src) {
// 	if (this != &src) {
// 		this->server_fd = src.server_fd;
// 		this->address = src.address;
// 		this->configuration = src.configuration;
// 		this->openFileDescriptors = src.openFileDescriptors;
// 	}
// 	return (*this);
// }

// HttpServer::HttpServer(const HttpServerConfiguration & config) {
// 	this->configuration = config;
// 	this->initializeSocket();
// }



// HttpServer::~HttpServer(void) {
// 	close(server_fd); // Execution should not end but closing server would be the way to do it
// }

// void HttpServer::announce(std::ostream & rhs) const{
// 	rhs << "HTTP Server: "
// 		<< "Server FD - " << this->server_fd
// 		<< std::endl;;
// }


// std::ostream& operator<<(std::ostream & out, HttpServer const & rhs)
// {
// 	rhs.announce(out);
// 	return (out);
// }