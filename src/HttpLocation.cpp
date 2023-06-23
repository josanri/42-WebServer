#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <vector>

#include "HttpLocation.hpp"

HttpLocation::HttpLocation(void) {
    this->directoryListing = false;
	this->upload = false;
}

HttpLocation::~HttpLocation() {
    
}
