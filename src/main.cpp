#include <poll.h>

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <cstdlib>

#include "HttpServer.hpp"
#include "HttpPortListener.hpp"

#define POLL_TIMEOUT 1000

int fake_parse(char *filename){
	(void) filename;
	return 0;
}

bool fake_portListeners(std::vector<HttpPortListener *> & vector, std::map<int,HttpPortListener *> & fileDescriptoToPort)
{
	HttpPortListener *listener = new HttpPortListener(fileDescriptoToPort);
	try {
		listener->initializeSocket();
		vector.push_back(listener);
	} catch (std::exception) {
		std::cerr << __func__ << ":" << __LINE__ << ": error when initializating a socket endpoint" << std::endl;
		return (false);
	}
	return (true);
}

int getNumberOfFds(std::vector<HttpPortListener *> vector) {
	int acc_size = 0;
	for (std::vector<HttpPortListener *>::iterator it = vector.begin(); it !=  vector.end(); it++)
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
	std::vector<HttpPortListener *> serverVector;
	std::map<int,HttpPortListener *> fileDescriptoToPort;
	if (argc != 2) {
		std::cerr << "usage: ./webserv configuration_file" << std::endl;
		return (1);
	}
	fake_parse(argv[1]);
	if (!fake_portListeners(serverVector, fileDescriptoToPort)) {
		std::cerr << __func__ << ":" << __LINE__ << ": not all the socket could be initialized, exiting the program" << std::endl;
		return (1);
	}
	while (true) {
		nfds_t open_fds_n = getNumberOfFds(serverVector);
		struct pollfd* polling_fds = createPollStruct(open_fds_n, serverVector);

		std::cout << "Number of file descriptors open: " << open_fds_n << std::endl;
		int err = poll(polling_fds, open_fds_n, POLL_TIMEOUT);
		for (nfds_t i = 0; i < open_fds_n; i++) {
			std::cout << "\tfd=" << polling_fds[i].fd << ", events:" 
					<< ((polling_fds[i].revents & POLLIN)  ? "POLLIN "  : "")
					<< ((polling_fds[i].revents & POLLHUP) ? "POLLHUP " : "")
					<< ((polling_fds[i].revents & POLLERR) ? "POLLERR " : "")
					<< ((polling_fds[i].revents & POLLOUT) ? "POLLOUT " : "") << std::endl;
		}
		if (err == 0) {
			std::cout << "\tPoll time out" << std::endl;
		} else if (err < 0) {
			std::cout << "\tI/O error" << std::endl;
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
