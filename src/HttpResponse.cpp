#include "HttpResponse.hpp"
#include <map>
#include <iostream>
#include <sstream>

static const struct tm * getTimePointer()
{
	time_t rawtime;
	time ( &rawtime );
	return localtime ( &rawtime );
}

HttpResponse::HttpResponse()
{
	std::string dateAux =  std::string(asctime(getTimePointer()));
	this->headers["Date"] = dateAux.substr(0, dateAux.size() - 1);
	this->setResponse("Default response");
}

HttpResponse::~HttpResponse()
{

}

HttpResponse & HttpResponse::operator=(const HttpResponse & rhs)
{
	if (this != &rhs)
	{
		this->headers = rhs.headers;
		this->response = rhs.response;
		this->statusMessage = rhs.statusMessage;
	}
	return (*this);
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

std::string HttpResponse::getResponse() const
{
	return (this->response);
}

void HttpResponse::setResponse(std::string response)
{
	if (response.size() > 0) {
		std::stringstream ss;
    ss << response.size();
		this->addHeader("Content-Length", ss.str());
	}
	
	this->response = response;
}

void HttpResponse::setStatusMessage(std::string statusMessage)
{
	this->statusMessage = statusMessage;
}

void HttpResponse::buildResponse()
{
	std::string responseWithoutBody = "HTTP/1.1 " + this->statusMessage + "\r\n";

	for (std::map<std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); it++)
		responseWithoutBody = responseWithoutBody + it->first + ": " + it->second + "\r\n";

	this->response = responseWithoutBody + "\r\n" + this->response;
}
