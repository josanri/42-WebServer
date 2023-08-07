#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>

#include <iostream>
#include <map>
#include <algorithm>

#include "HttpPortListener.hpp"
#include "HttpResponse.hpp"
#include "HttpServer.hpp"
#include "HttpRequest.hpp"

#define BUFFER_SIZE 8192
#define LISTEN_BACKLOG 20

// Debug
void replaceAll(std::string & str, std::string const & s1, std::string const & s2)
{
    size_t pos = 0;
    while ((pos = str.find(s1, pos)) != std::string::npos)
    {
        str.erase(pos, s1.length());
        str.insert(pos, s2);
        pos += s2.length();
    }
}

void HttpPortListener::addConnection(int fd)
{
	this->fileDescriptorToPort[fd] = this;
	this->openFileDescriptors.insert(fd);
}


void HttpPortListener::closeConnection(int fd)
{
	close(fd);
	this->fileDescriptorToPort.erase(fd);
	this->openFileDescriptors.erase(fd);
}

static int configSocketOptions(int server_fd) {
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) { // Configurating port
		std::cerr << __func__ << ":" << __LINE__ << ": error at configurating the socket reusing port directive" << std::endl;
		return (EXIT_FAILURE);
	}
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){ // Configurating address
		std::cerr << __func__ << ":" << __LINE__ << ": error at configurating the socket reusing addresss directive" << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

int HttpPortListener::bindServerConnection() {
	if (this->getPort() < 0 || this->getPort() > 65535)
		return (EXIT_FAILURE);
	if (configSocketOptions(this->server_fd)) {
		std::cerr << __func__ << ":" << __LINE__ << ": error when configurating the socket" << std::endl;
		return (EXIT_FAILURE);
	}
	bzero(&this->address, sizeof(this->address));
	this->address.sin_family = AF_INET;
	this->address.sin_addr.s_addr = INADDR_ANY;
	this->address.sin_port = htons(this->getPort());
	if (bind(server_fd, (struct sockaddr *) &address, sizeof(this->address))) {// Socket to local address
		std::cerr << __func__ << ":" << __LINE__ << ": error when binding the socket" << std::endl;
		return (EXIT_FAILURE);
	}
	if (listen(this->server_fd, LISTEN_BACKLOG)) {// Socket to passive mode
		std::cerr << __func__ << ":" << __LINE__ << ": error when turning socket into passive socket" << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

static int setTimeout(int fd){
	struct timeval timeout;      
	timeout.tv_sec = 10;
	timeout.tv_usec = 0;
	
	if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		std::cerr << __func__ << ":" << __LINE__ << ": error when setting socket receive timeout" << std::endl;
		return (EXIT_FAILURE);
	}
	if (setsockopt (fd, SOL_SOCKET, SO_SNDTIMEO, &timeout,	sizeof(timeout)) < 0) {
		std::cerr << __func__ << ":" << __LINE__ << ": error when setting socket send timeout" << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

static int setKeepAlive(int fd){
	int yes = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int)) < 0) {
		std::cerr << __func__ << ":" << __LINE__ << ": error when setting socket to keepalive mode" << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}

void HttpPortListener::initializeServerNamesToServer(const std::vector<HttpServer *> &servers) {
	for (std::vector<HttpServer *>::const_iterator it = servers.begin(); it != servers.end(); it++) {
		const std::vector<std::string> & serverNames = (*it)->getServerNames();
		for (std::vector<std::string>::const_iterator name_it = serverNames.begin(); name_it != serverNames.end(); name_it++) {
			this->serverNamesToServer[*name_it] = (*it);
		}
	}
}

void HttpPortListener::sendResponse(const int & fd) {
	if (this->fileDescriptorToRequest.find(fd) != this->fileDescriptorToRequest.end()) {
		HttpRequest & readyRequest = this->fileDescriptorToRequest.at(fd);
		const std::string & serverHost = readyRequest.getHost();
		HttpServer *httpServer;
		if (this->serverNamesToServer.find(serverHost) != this->serverNamesToServer.end()) {
			httpServer = this->serverNamesToServer.at(serverHost);
		} else {
			httpServer = serverNamesToServer.begin()->second;
		}
		HttpResponse response = httpServer->processHttpRequest(readyRequest);
		response.buildResponse(httpServer->getErrorNumberToLocation());
		std::cout << "\033[0;34mResponse: " << response.getResponse() << "\033[0m" << std::endl;
		ssize_t writen = send(fd, response.c_str(), response.size(), 0);
		if (writen != (ssize_t) response.size()) {
			std::cerr << "Error when sending the message" << std::endl;
		} else {
			std::cout << "\tMessageSent" << std::endl;
			// Remove request
			this->fileDescriptorToRequest.erase(fd);
		}
	} else {
	}			
}

void HttpPortListener::receiveRequest(const int & fd) {
	char read_buffer[BUFFER_SIZE];
	int len = recv(fd, read_buffer, BUFFER_SIZE, 0);
	if (len < 0) {
		std::cerr << "There was an error reading from a socket, closing connection" << std::endl;
		this->closeConnection(fd);
	} else if (len == 0) {
		// User closed connection
		this->closeConnection(fd);
	} else {
		// Receive connection
		read_buffer[len] = 0;
		std::string request = read_buffer;
		if (this->fileDescriptorToRequest.find(fd) != this->fileDescriptorToRequest.end()) {
			HttpRequest & inProcessRequest = this->fileDescriptorToRequest.at(fd);
			inProcessRequest.append(request);
		} else {
			this->fileDescriptorToRequest[fd] = HttpRequest();
			this->fileDescriptorToRequest[fd].append(request);
		}
		/* Debug */
		std::string request_debug = request;
		replaceAll(request_debug, "\r\n", "\\r\\n\n");
		std::cout << "\033[0;32mRequest: " << request_debug << "\033[0m" << std::endl;
		/* */
	}
}

void HttpPortListener::acceptConnection(const int & fd) {
    socklen_t address_len = sizeof(this->address);
	int new_socket_fd;
	
	new_socket_fd = accept(fd, (struct sockaddr *) &address, &address_len);
	if (new_socket_fd == -1) {
		std::cerr << "Error when trying to accept a connection " << std::endl;
		close(fd);
	} else {
		if (fcntl(new_socket_fd, F_SETFL, O_NONBLOCK) == -1 || setTimeout(new_socket_fd) != 0 || setKeepAlive(new_socket_fd) != 0) {
			close(new_socket_fd);
		} else {
			std::cout << "New file_descriptor " << new_socket_fd << std::endl;
			this->addConnection(new_socket_fd);
		}
	}
}

void HttpPortListener::connect(const int & fd, const int & revents) {
	if (fd == this->server_fd) {
		if (revents & POLLIN) {
			this->acceptConnection(fd);
		}
	} else {
		if (revents & POLLHUP || revents & POLLERR) {
			this->closeConnection(fd);
		} else {
			if (revents & POLLIN) {
				this->receiveRequest(fd);
			} else if (revents & POLLOUT) {
				this->sendResponse(fd);
			}
		}
    }// Accept connection if none is present
}

void HttpPortListener::initializeSocket(){
	this->server_fd = socket(AF_INET, SOCK_STREAM, 0); // Endpoint
	if (this->server_fd == -1) {
		std::cerr << __func__ << ":" << __LINE__ << ": error when creating the socket endpoint" << std::endl;
		throw std::exception();
	}
	if (fcntl(this->server_fd, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << __func__ << ":" << __LINE__ << ": error when turning socket into non blocking" << std::endl;
		close(server_fd);
		throw std::exception();
	} 
	if (this->bindServerConnection()) {
		std::cerr << __func__ << ":" << __LINE__ << ": error when turning socket into passive socket" << std::endl;
		close(server_fd);
		throw std::exception();
	}
	this->addConnection(this->server_fd);
}

HttpPortListener::HttpPortListener(int port, std::map<int, HttpPortListener*> & map, const std::vector<HttpServer *> & servers) : fileDescriptorToPort(map) {
	this->port = port;
	this->initializeServerNamesToServer(servers);
}

int HttpPortListener::getServerFd() const {
	return (this->server_fd);
}

int HttpPortListener::getPort() const {
	return (this->port);
}

std::set<int> & HttpPortListener::getOpenFileDescriptors(void) {
	return (this->openFileDescriptors);
}

HttpPortListener::~HttpPortListener(){
	
}