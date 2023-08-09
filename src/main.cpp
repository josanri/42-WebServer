#include <poll.h>
#include <unistd.h>

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

void insertAllServers(std::set<HttpServer *>  & serverSet, std::vector<HttpPortListener *> &serverVector) {
	for (std::vector<HttpPortListener *>::iterator iter = serverVector.begin(); iter != serverVector.end(); ++iter)
	{
		std::vector<HttpServer *> serverVector = (*iter)->getServer();
		for (std::vector<HttpServer *>::iterator iterat2 = serverVector.begin(); iterat2 != serverVector.end(); ++iterat2)
		{
			serverSet.insert(*iterat2);
		}
	}
}

int getNumberOfFds(std::vector<HttpPortListener *> const & vector) {
	int acc_size = 0;
	for (std::vector<HttpPortListener *>::const_iterator it = vector.begin(); it !=  vector.end(); it++)
	{
		acc_size += (*it)->getOpenFileDescriptors().size();
	}
	return (acc_size);
}

void freeMemory(std::vector<HttpPortListener *>  & portListenerVector)
{
	std::set<HttpServer *>  serverSet;
	insertAllServers(serverSet, portListenerVector);
	for (std::set<HttpServer *>::iterator serverIterator = serverSet.begin(); serverIterator != serverSet.end(); ++serverIterator)
	{
		std::vector<HttpLocation *> serverLocationVector = (*serverIterator)->getLocations();
		for (std::vector<HttpLocation *>::iterator locationIterator = serverLocationVector.begin(); locationIterator != serverLocationVector.end(); ++locationIterator)
		{
			delete *locationIterator;
		}
		delete *serverIterator;
	}
	for (std::vector<HttpPortListener *>::iterator portListenerIterator = portListenerVector.begin(); portListenerIterator != portListenerVector.end(); ++portListenerIterator)
	{
		std::set<int> & openFileDescriptors = (*portListenerIterator)->getOpenFileDescriptors();
		for (std::set<int>::iterator fd_it = openFileDescriptors.begin(); fd_it != openFileDescriptors.end(); fd_it++)
		{
			close(*fd_it);
		}
		delete *portListenerIterator;
	}
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
		int err = poll(polling_fds, open_fds_n, POLL_TIMEOUT);
		if (err == 0) {
			std::cerr << "\033[0;33mPoll time out\033[0m" << std::endl;
		} else if (err < 0) {
			std::cerr << "\033[0;33mPoll I/O error\033[0m" << std::endl;
		} else {
			for (nfds_t i = 0; i < open_fds_n; i++) {
				HttpPortListener * httpPortListener = fileDescriptoToPort.at(polling_fds[i].fd);
				httpPortListener->connect(polling_fds[i].fd,  polling_fds[i].revents);
			}
		}
		delete [] polling_fds;
	}
	freeMemory(portListenerVector);
	return (0);
}
