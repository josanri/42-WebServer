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
#include <map>
#include "HttpPortListener.hpp"

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
	// this->configuration.getNumberOfConections()
	if (listen(this->server_fd, 5)) {// Socket to local address
		std::cerr << __func__ << ":" << __LINE__ << ": error when turning socket into passive socket" << std::endl;
		return (EXIT_FAILURE);
	}
	return (0);
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
	return (0);
}

void HttpPortListener::connect(const int & fd, const int & revents) {
    socklen_t address_len = sizeof(this->address);
    int new_socket_fd;

	std::cout << "\tfd=" << fd << ", events:" 
						<< ((revents & POLLIN)  ? "POLLIN "  : "")
						<< ((revents & POLLHUP) ? "POLLHUP " : "")
						<< ((revents & POLLERR) ? "POLLERR " : "")
						<< ((revents & POLLOUT) ? "POLLOUT " : "") << std::endl;
	if (fd == this->server_fd) {
		if (revents & POLLIN) {
			new_socket_fd = accept(fd, (struct sockaddr *) &address, &address_len);
			if (new_socket_fd != -1) {
				fcntl(new_socket_fd, F_SETFL, O_NONBLOCK);
				setTimeout(fd);
				int yes = 1;
				setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int));// Keep alive True
				std::cout << "New file_descriptor " << new_socket_fd << std::endl;
				this->addConnection(new_socket_fd);
			} else {
				std::cerr << "Error when trying to accept a connection " << std::endl;
				this->closeConnection(fd);
			}
		}
	}
	else
	{
		char read_buffer[BUFFER_SIZE];
		if (revents & POLLHUP || revents & POLL_ERR) {
			this->closeConnection(fd);
			return;
		}
		if (revents & POLLIN) {
			int len = recv(fd, read_buffer, BUFFER_SIZE, 0);
			if (len < 0) {
				std::cout << "There was an error reading from a socket, closing connection" << std::endl;
				this->closeConnection(fd);
			} else if (len == 0) {
				// User closed connection
				this->closeConnection(fd);
			} else {
				// Receive connection
				std::string request = read_buffer;
				std::cout << request << std::endl;
			}
		}
		if (revents & POLLOUT) {
		}
	}
    // Accept connection if none is present, 
}

void HttpPortListener::initializeSocket(){
	this->server_fd = socket(AF_INET, SOCK_STREAM, 0); // Endpoint
	if (this->server_fd == -1) {
		std::cerr << __func__ << ":" << __LINE__ << ": error when creating the socket endpoint" << std::endl;
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

HttpPortListener::HttpPortListener(std::map<int, HttpPortListener*> & map) : fileDescriptorToPort(map) {
	this->port = 8080;
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
