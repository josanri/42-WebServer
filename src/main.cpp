#include <poll.h>

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <cstdlib>

#include "HttpServer.hpp"
#include "HttpLocation.hpp"
#include "HttpPortListener.hpp"
#include "Parser.hpp"

#define POLL_TIMEOUT 1000

int getNumberOfFds(std::vector<HttpPortListener *> const & vector) {
	int acc_size = 0;
	for (std::vector<HttpPortListener *>::const_iterator it = vector.begin(); it !=  vector.end(); it++)
	{
		acc_size += (*it)->getOpenFileDescriptors().size();
	}
	return (acc_size);
}

struct pollfd* createPollStruct(nfds_t open_fds, std::vector<HttpPortListener *> &serverVector){
	int counter = 0;
	struct pollfd* polling_fds = new struct pollfd[open_fds];
	for (std::vector<HttpPortListener *>::iterator it = serverVector.begin(); it !=  serverVector.end(); it++)
	{
		std::set<int> & openFileDescriptors = (*it)->getOpenFileDescriptors();
		for (std::set<int>::iterator fd_it = openFileDescriptors.begin(); fd_it != openFileDescriptors.end(); fd_it++)
		{
			polling_fds[counter].events = POLLIN | POLLOUT;
			polling_fds[counter].fd = *fd_it;
			counter++;
		}
	}
	return (polling_fds);
}

int main(int argc, char **argv)
{
	std::vector<HttpPortListener *> portListenerVector;
	std::map<int,HttpPortListener *> fileDescriptoToPort; // Access in O(log n) instead of O(n)
	try {
		if (argc != 2) {
			throw std::runtime_error("usage: ./webserv configuration_file");
		}
		Parser parser(argv[1]);
		parser.parse(portListenerVector, fileDescriptoToPort);
	} catch (std::exception & e) {
		std::cerr << e.what() << std::endl;
		return (1);
	}
	while (true) {
		nfds_t open_fds_n = getNumberOfFds(portListenerVector);
		struct pollfd* polling_fds = createPollStruct(open_fds_n, portListenerVector);

		// std::cout << "Number of file descriptors open: " << open_fds_n << std::endl;
		int err = poll(polling_fds, open_fds_n, POLL_TIMEOUT);
		if (err == 0) {
			std::cerr << "Poll time out" << std::endl;
		} else if (err < 0) {
			std::cerr << "Poll I/O error" << std::endl;
		} else {
			for (nfds_t i = 0; i < open_fds_n; i++) {
				HttpPortListener * httpPortListener = fileDescriptoToPort.at(polling_fds[i].fd);
				httpPortListener->connect(polling_fds[i].fd,  polling_fds[i].revents);
			}
		}
		delete [] polling_fds;
	}
	return (0);
}
