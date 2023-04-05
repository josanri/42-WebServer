#include <iostream>
#include <list>
#include <map>
#include <set>
#include <fstream>
#include "HttpServer.hpp"
#include "HttpServerConfiguration.hpp"

std::list<HttpServerConfiguration> read_configuration_file(const char *configuration_filename)
{
	std::list<HttpServerConfiguration> config;
	std::ifstream file(configuration_filename);
	
	// Do stuff

	// File is closed as RAII pattern is implemented in ifstream
	return (config);
}

std::list<HttpServer> initializeServers(std::list<HttpServerConfiguration> & list)
{
	std::list<HttpServer> serverList;
	for (std::list<HttpServerConfiguration>::iterator it; it != list.end(); it++) {
		HttpServer newServer(*it);
		serverList.push_back(newServer);
	}
	return (serverList);
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		std::cerr << "usage: ./webserv configuration_file" << std::endl;
		return (1);
	}
	std::list<HttpServerConfiguration> server_configuration_list = read_configuration_file(argv[1]);
	std::map<HttpServer, std::set<int> &> serverToOpenFileDescriptors;
	(void) serverToOpenFileDescriptors;
	while (true) {
		// On each iteration

		// Get all file descriptors into an struct array

		// Use poll

		// Do the operations depending on the server they came from
	}
	
	return 0;
}
