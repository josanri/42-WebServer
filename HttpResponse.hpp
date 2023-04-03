#ifndef HTTP_RESPONSE_HPP
# define HTTP_RESPONSE_HPP
# include <map>

class HttpResponse
{
	private:
		std::map<std::string, std::string> headers;
	public:
		HttpResponse();
		HttpResponse(const HttpResponse &);
		HttpResponse & operator=(const HttpResponse &);
		~HttpResponse();
};

#endif