#include "HttpRequest.hpp"
#include <map>
#include <iostream>

HttpRequest::HttpRequest() {

}

HttpRequest::~HttpRequest() {

}

void HttpRequest::append(std::string & str)
{
    this->full_request.append(str);
    if (this->state == HttpRequest::HEADERS_NOT_FINISHED) {
        this->crlfcrlf = this->full_request.find("\r\n\r\n");
        if (this->crlfcrlf != std::string::npos) {
            if (this->full_request.find("GET", 0, 3) != std::string::npos) {
                this->state = HttpRequest::FINISHED;
                // TODO -Parse Request
            } else if (this->full_request.find("POST", 0, 4) != std::string::npos
                || this->full_request.find("PUT", 0, 3) != std::string::npos
                || this->full_request.find("DELETE", 0, 6) != std::string::npos)
            {
                this->state = HttpRequest::BODY_NOT_FINISHED;
                // TODO - Parse Request
            } else {
                this->state = HttpRequest::ERROR; // Unrecognized metod
            }
        }
    }
    if (this->state == HttpRequest::BODY_NOT_FINISHED) {
        // Count the length of the body according to the content length
        if (this->full_request.size() == this->contentLength + this->crlfcrlf + 4)
            this->state = HttpRequest::FINISHED;
        else if (this->full_request.size() > this->contentLength + this->crlfcrlf + 4)
            this->state = HttpRequest::ERROR; // Content length larger
    }
}

const std::string & HttpRequest::getHost() const
{
    return this->host;
}
