#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP
# include <iostream>
# include <map>
# include <string>


class HttpRequest {
	enum HttpRequestState {
		HEADERS_NOT_FINISHED,
		CHUNKS_NOT_FINISHED, // Transfer Encoding chunks requests
		BODY_NOT_FINISHED, // Post requests
		FINISHED,
		ERROR,
	};
	private:
		std::string method;
		std::string route;
		std::string httpVersion;
		std::map<std::string, std::string> headers;
		std::string body;
		std::string full_request;
		size_t crlfcrlf;
		size_t contentLength;
		HttpRequestState state;
		void parseHeadersKeyValue(size_t first_pos, size_t second_pos);

	public:
		HttpRequest();
		HttpRequest(const HttpRequest &);
		HttpRequest & operator=(const HttpRequest &);
		~HttpRequest();
		void append(std::string & str);
		void parseHeaders();
		std::string getMethod();
		std::string getRoute();
		std::string getHost();
		std::string getBody();
		size_t getContentLength();

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