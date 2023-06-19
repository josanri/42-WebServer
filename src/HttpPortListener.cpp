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
#include "HttpPortListener.hpp"

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

int HttpPortListener::prepareServerConnection() {
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

void HttpPortListener::connect(const int & fd, const int & revents) {
    socklen_t address_len = sizeof(this->address);
    int new_socket_fd;

	printf("\tfd=%d; events: %s%s%s%s\n", fd,
						(revents & POLLIN)  ? "POLLIN "  : "",
						(revents & POLLHUP) ? "POLLHUP " : "",
						(revents & POLLERR) ? "POLLERR " : "",
						(revents & POLLOUT) ? "POLLOUT " : "");
	if (fd == this->server_fd) {
		if (revents & POLLIN){
			new_socket_fd = accept(fd, (struct sockaddr *) &address, &address_len);
			if (new_socket_fd != -1) {
				fcntl(this->server_fd, F_SETFL, O_NONBLOCK);
				this->openFileDescriptors.insert(new_socket_fd);
			}
		}
	}
	else
	{
		char read_buffer[8192];
		if (revents & POLLIN){
			int len = recv(fd, read_buffer, 8192, 0);
			write(0, read_buffer, len);
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
	if (this->prepareServerConnection()) {
		std::cerr << __func__ << ":" << __LINE__ << ": error when turning socket into passive socket" << std::endl;
		close(server_fd);
		throw std::exception();
	}
	this->openFileDescriptors.insert(this->server_fd);
}

HttpPortListener::HttpPortListener(void) {
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
