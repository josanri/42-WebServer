#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include "HttpServer.hpp"
#include "HttpServerConfiguration.hpp"

std::vector<HttpServerConfiguration> parseConfigurationFile(const char *configuration_filename)
{
	std::vector<HttpServerConfiguration> config;
	std::ifstream file(configuration_filename);
	
	// Do stuff

	// File is closed as RAII pattern is implemented in ifstream
	return (config);
}

std::vector<HttpServer> initializeServers(std::vector<HttpServerConfiguration> & vector)
{
	std::vector<HttpServer> serverVector;
	for (std::vector<HttpServerConfiguration>::iterator it; it != vector.end(); it++) {
		HttpServer newServer(*it);
		serverVector.push_back(newServer);
	}
	return (serverVector);
}

int main(int argc, char **argv)
{
	if (argc != 2) {
		std::cerr << "usage: ./webserv configuration_file" << std::endl;
		return (1);
	}
	std::vector<HttpServerConfiguration> serverConfigurationVector = parseConfigurationFile(argv[1]);
	std::vector<HttpServer> serverVector = initializeServers(serverConfigurationVector);
	std::map<HttpServer, std::set<int> &> serverToOpenFileDescriptors;
	(void) serverToOpenFileDescriptors;
	// serverToOpenFileDescriptors[serverVector[0]] = serverVector[0].getOpenFileDescriptors();
	while (true) {
		// On each iteration

		// Get all file descriptors into an struct array

		// Use poll
		
		// Do the operations depending on the server they came from
	}
	
	return 0;
}
