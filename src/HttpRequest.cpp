#include "HttpRequest.hpp"
#include <map>
#include <iostream>

HttpRequest::HttpRequest()
{

}

HttpRequest::~HttpRequest()
{

}

void HttpRequest::append(std::string & str)
{
    this->full_request.append(str);
}

const std::string & HttpRequest::getHost() const
{
    return this->host;
}
