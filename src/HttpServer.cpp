#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

static void config_socket_address(struct sockaddr_in * address)
{
	int port = 8080;

	bzero(address, sizeof(*address));
	address->sin_family = AF_INET;
	address->sin_addr.s_addr = INADDR_ANY;
	address->sin_port = htons(port);
}

static int config_socket_options(int server_fd) {
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) { // Configurating port
		std::cerr << "Error at configurating the socket reusing port directive" << std::endl;
		return (1);
	}
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))){ // Configurating address
		std::cerr << "Error at configurating the socket reusing addresss directive" << std::endl;
		return (1);
	}
	return (0);
}

int server_daemon()
{
	int server_fd;
	int number_of_conections = 2;
	struct sockaddr_in address;
	socklen_t address_len = sizeof(address);

	server_fd = socket(AF_INET, SOCK_STREAM, 0); // Endpoint
	if (server_fd == -1) {
		std::cerr << __func__ << ":" << __LINE__ << ": error when creating the socket endpoint" << std::endl;
		return (1);
	}
	if (config_socket_options(server_fd)) {
		std::cerr << __func__ << ":" << __LINE__ << ": error when configurating the socket" << std::endl;
		close(server_fd);
		return (1);
	}
	config_socket_address(&address);
	if (bind(server_fd, (struct sockaddr *) &address, address_len)) {// Socket to local address
		std::cerr << __func__ << ":" << __LINE__ << ": error when binding the socket" << std::endl;
		close(server_fd);
		return (1);
	}
	if (listen(server_fd, number_of_conections)) {// Socket to local address
		std::cerr << __func__ << ":" << __LINE__ << ": error when turning socket into passive socket" << std::endl;
		close(server_fd);
		return (1);
	}
	int new_socket_fd = accept(server_fd, (struct sockaddr *) &address, &address_len); // Accept connection
	std::cout << "Socket " << new_socket_fd << " accepted the message." << std::endl;
	fcntl(new_socket_fd, F_SETFL, O_NONBLOCK); //  Non blocking fd
	char buffer[2048];
	int readed;
	readed = read(new_socket_fd, buffer, 2047);
	buffer[readed] = '\0';
	std::cout << buffer << std::endl;
	close(new_socket_fd);
	close(server_fd); // Execution should not end but closing server would be the way to do it
	return (0);
}