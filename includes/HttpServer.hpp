#ifndef HTTP_SERVER_HPP
# define HTTP_SERVER_HPP

# include <vector>
# include <string>

# include "HttpResponse.hpp"
# include "HttpRequest.hpp"

class HttpServer
{
	private:
        std::string host;
		std::vector<int> ports;
	public:
		std::string getHost() const;
		const std::vector<int> & getPorts() const;
		
		HttpServer(void);
		~HttpServer(void);
		HttpResponse processHttpRequest(HttpRequest &);
};

#endif

