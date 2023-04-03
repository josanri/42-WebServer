#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP
# include <map>

class HttpRequest
{
	private:
		std::map<std::string, std::string> headers;
	public:
		HttpRequest();
		HttpRequest(const HttpRequest &);
		HttpRequest & operator=(const HttpRequest &);
		~HttpRequest();
};

#endif