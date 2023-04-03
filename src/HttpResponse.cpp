#include "HttpResponse.hpp"
#include <map>
#include <iostream>

static const struct tm * getTimePointer()
{
	time_t rawtime;
	struct tm * timeinfo;

	time ( &rawtime );
	return localtime ( &rawtime );
}

HttpResponse::HttpResponse()
{
	this->headers["date"] = std::string(asctime(getTimePointer()));
}