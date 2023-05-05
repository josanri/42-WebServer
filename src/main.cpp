#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <cstdlib>
#include "HttpServer.hpp"
#include "HttpServerConfiguration.hpp"
#include "HttpPortListener.hpp"
#include <poll.h>

// std::vector<HttpServerConfiguration> parseConfigurationFile(const char *configuration_filename)
// {
// 	std::vector<HttpServerConfiguration> config;
// 	std::ifstream file(configuration_filename);
	
// 	// Do stuff

// 	// File is closed as RAII pattern is implemented in ifstream
// 	return (config);
// }

// std::vector<HttpPortListener> initializeServers(std::vector<HttpServerConfiguration> & vector)
// {
// 	std::vector<HttpPortListener> serverVector;
// 	for (std::vector<HttpServerConfiguration>::iterator it = vector.begin(); it != vector.end(); it++) {
// 		//serverVector.push_back(newServer);
// 	}
// 	return (serverVector);
// }

void fakePortListeners(std::vector<HttpPortListener *> & vector)
{
	HttpPortListener *listener = new HttpPortListener();
	listener->initializeSocket();
	vector.push_back(listener);

}


int getNumberOfFds(std::vector<HttpPortListener *> vector) {
	int acc_size = 0;
	for (std::vector<HttpPortListener *>::iterator it = vector.begin(); it !=  vector.end(); it++)
	{
		acc_size += (*it)->getOpenFileDescriptors().size();
	}
	return (acc_size);
}

void fillPollingStructure(struct pollfd* polling_fds, std::vector<HttpPortListener *> &serverVector){
	int counter = 0;
	for (std::vector<HttpPortListener *>::iterator it = serverVector.begin(); it !=  serverVector.end(); it++)
	{
		std::set<int> & openFileDescriptors = (*it)->getOpenFileDescriptors();
		for (std::set<int>::iterator fd_it = openFileDescriptors.begin(); fd_it != openFileDescriptors.end(); fd_it++)
		{
			polling_fds[counter].events = POLL_IN | POLL_OUT;
			polling_fds[counter].fd = *fd_it;
		}
	}
}

int main(int argc, char **argv)
{
	std::vector<HttpPortListener *> serverVector;
	(void) argv;
	fakePortListeners(serverVector);
	if (argc != 2) {
		std::cerr << "usage: ./webserv configuration_file" << std::endl;
		return (1);
	}
	// std::vector<HttpServerConfiguration> serverConfigurationVector = parseConfigurationFile(argv[1]);
	// std::vector<HttpPortListener> serverVector = initializeServers(serverConfigurationVector);
	// (void) serverVector;
	// serverToOpenFileDescriptors[serverVector[0]] = serverVector[0].getOpenFileDescriptors();
	while (true) {
		nfds_t open_fds_n = getNumberOfFds(serverVector);
		// Use poll
		struct pollfd* polling_fds = new struct pollfd[open_fds_n];
		fillPollingStructure(polling_fds, serverVector);
		int err = poll(polling_fds, open_fds_n, 0);
		if (err == 0)
			std::cout << "Time limit" << std::endl;
		else if (err < 0)
			std::cout << "I/O error" << std::endl;
		// Get all file descriptors into an struct array

		
		// Do the operations depending on the server they came from
		delete [] polling_fds;
	}
	
	return 0;
}
