#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP
# include <iostream>
# include <map>

class HttpRequest
{
	private:
        std::string method;
        std::string route;
        std::string host;
		std::map<std::string, std::string> headers;
		std::string body;
		std::string full_request;
		bool isFinished;
	public:
		HttpRequest();
		HttpRequest(const HttpRequest &);
		HttpRequest & operator=(const HttpRequest &);
		~HttpRequest();
		void append(std::string & str);
		const std::string & getHost() const;
};

/*
Example:

GET /hello.htm HTTP/1.1
User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)
Host: www.google.com
Accept-Language: en-us
Accept-Encoding: gzip, deflate
Connection: Keep-Alive

*/
#endif