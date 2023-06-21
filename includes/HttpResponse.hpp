#ifndef HTTP_RESPONSE_HPP
# define HTTP_RESPONSE_HPP
# include <map>
# include <iostream>

class HttpResponse
{
	private:
		std::map<std::string, std::string> headers;
		std::string response;
	public:
		HttpResponse();
		HttpResponse & operator=(const HttpResponse &);
		~HttpResponse();
		const char * c_str();
		size_t size() const;
};

#endif