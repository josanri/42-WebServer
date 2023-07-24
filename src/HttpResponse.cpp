#include "HttpResponse.hpp"
#include <map>
#include <iostream>

static const struct tm * getTimePointer()
{
	time_t rawtime;
	time ( &rawtime );
	return localtime ( &rawtime );
}

HttpResponse::HttpResponse()
{
	this->headers["Date"] = std::string(asctime(getTimePointer()));
	this->response = "hola";
}

HttpResponse::~HttpResponse()
{

}

void HttpResponse::setStatusCode(int statusCode)
{
	this->statusCode = statusCode;
}

void HttpResponse::addHeader(std::string key, std::string value)
{
	this->headers[key] = value;
}

const char *HttpResponse::c_str()
{
	return (this->response.c_str());
}

size_t HttpResponse::size() const
{
	return (this->response.size());
}