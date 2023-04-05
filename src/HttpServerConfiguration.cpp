#include "HttpServerConfiguration.hpp"


HttpServerConfiguration::HttpServerConfiguration(/* args */) {
	this->port = 8080;
}

HttpServerConfiguration::~HttpServerConfiguration()
{
}

int HttpServerConfiguration::getNumberOfConections() const {
	return (this->number_of_conections);
}

int HttpServerConfiguration::getPort() const {
	return (this->port);
}