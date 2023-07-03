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

#define POLL_TIMEOUT 1000

bool fake_parse(char *filename, std::vector<HttpServer *> servers){
	(void) filename;
	// Parse
	try {
		for (int i = 0; i < 1; i++) {
			std::vector<int>ports;
			ports.push_back(8080);

			std::vector<std::string>serverNames;
			serverNames.push_back("localhost");

			std::vector<HttpLocation *> locations;
			std::string route = "./web-test";
			std::vector<std::string> methods;
			methods.push_back("GET");
			methods.push_back("POST");
			std::string defaultFile = "";
			std::string redirectionRoute = "";
			std::map<std::string, std::string> fileExtensionToCGI;
			bool directoryListing = false;
			bool upload = false;
			HttpLocation *location_1 = new HttpLocation(route, methods, defaultFile, redirectionRoute, fileExtensionToCGI, directoryListing, upload);
			locations.push_back(location_1);

			std::map<int, std::string> errorNumberToLocation;
			// Empty
			
			unsigned int maxBody = 1500;
			HttpServer *newServer = new HttpServer(ports, serverNames, locations, errorNumberToLocation, maxBody);
			servers.push_back(newServer);
		}
	} catch (std::exception & e) {
		std::cerr << __func__ << ":" << __LINE__ << ": error when parsing the configuration file" << std::endl;
		return false;
	}
	return true;
}

bool fake_portListeners(std::vector<HttpPortListener *> & portListenerVector, std::map<int,HttpPortListener *> & fileDescriptoToPort, std::vector<HttpServer *> & serverVector)
{

	HttpPortListener *listener = new HttpPortListener(8080, fileDescriptoToPort, serverVector);
	try {
		listener->initializeSocket();
		portListenerVector.push_back(listener);
	} catch (std::exception & e) {
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
	std::vector<HttpServer *> serverVector;
	std::vector<HttpPortListener *> portListenerVector;
	std::map<int,HttpPortListener *> fileDescriptoToPort; // Access in O(log n) instead of O(n)
	if (argc != 2) {
		std::cerr << "usage: ./webserv configuration_file" << std::endl;
		return (1);
	}
	if (!fake_parse(argv[1], serverVector)) {
		std::cerr << __func__ << ":" << __LINE__ << ": error while parsing, exiting the program" << std::endl;
		return (1);
	}
	if (!fake_portListeners(portListenerVector, fileDescriptoToPort, serverVector)) {
		std::cerr << __func__ << ":" << __LINE__ << ": not all the socket could be initialized, exiting the program" << std::endl;
		return (1);
	}
	while (true) {
		nfds_t open_fds_n = getNumberOfFds(portListenerVector);
		struct pollfd* polling_fds = createPollStruct(open_fds_n, portListenerVector);

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
