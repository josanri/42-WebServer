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

static int config_socket_options(int server_fd) {
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

int HttpServer::prepare_server_connection() {
	if (config_socket_options(this->server_fd)) {
		std::cerr << __func__ << ":" << __LINE__ << ": error when configurating the socket" << std::endl;
		return (EXIT_FAILURE);
	}
	bzero(&this->address, sizeof(this->address));
	this->address.sin_family = AF_INET;
	this->address.sin_addr.s_addr = INADDR_ANY;
	this->address.sin_port = htons(this->port);
	if (bind(server_fd, (struct sockaddr *) &address, sizeof(this->address))) {// Socket to local address
		std::cerr << __func__ << ":" << __LINE__ << ": error when binding the socket" << std::endl;
		return (EXIT_FAILURE);
	}
	if (listen(this->server_fd, this->number_of_conections)) {// Socket to local address
		std::cerr << __func__ << ":" << __LINE__ << ": error when turning socket into passive socket" << std::endl;
		return (EXIT_FAILURE);
	}
	return (0);
}

/*
int server_daemon()
{
	struct sockaddr_in address;
	socklen_t address_len = sizeof(address);


	std::cout << "Socket " << new_socket_fd << " accepted the message." << std::endl;
	fcntl(new_socket_fd, F_SETFL, O_NONBLOCK); //  Non blocking fd
	// char buffer[2048];
	// int readed;
	// readed = read(new_socket_fd, buffer, 2047);
	// buffer[readed] = '\0';
	// std::cout << buffer << std::endl;
	// close(new_socket_fd);
}
*/
HttpServer::HttpServer(void) {
	this->port = 8080;
	this->number_of_conections = 2;

	this->server_fd = socket(AF_INET, SOCK_STREAM, 0); // Endpoint
	if (this->server_fd == -1) {
		std::cerr << __func__ << ":" << __LINE__ << ": error when creating the socket endpoint" << std::endl;
	}
	fcntl(this->server_fd, F_SETFL, O_NONBLOCK);
	if (this->prepare_server_connection()) {
		std::cerr << __func__ << ":" << __LINE__ << ": error when turning socket into passive socket" << std::endl;
		close(server_fd);
		throw std::exception();
	}
	/*
	socklen_t address_len = sizeof(this->address);
	int new_socket_fd = -;
	new_socket_fd = accept(server_fd, (struct sockaddr *) &address, &address_len);
	if (new_socket_fd != -1) {
		fcntl(this->server_fd, F_SETFL, O_NONBLOCK);
	}
	// Accept connection if none is present, 
	std::cout << new_socket_fd << std::endl;
	*/

}

		
HttpServer::~HttpServer(void) {
	close(server_fd); // Execution should not end but closing server would be the way to do it
}
