#ifndef HTTP_SERVER_HPP
# define HTTP_SERVER_HPP

# include <vector>
# include <string>

# include "HttpResponse.hpp"
# include "HttpRequest.hpp"
# include "HttpLocation.hpp"

class HttpServer
{
	private:
        std::string host;
		unsigned int maxBody;
		std::vector<int> ports;
		std::vector<std::string> serverNames;
		std::vector<HttpLocation> ports;
		std::map<int, std::string> errorNumberToLocation
	public:
		std::string getHost() const;
		const std::vector<int> & getPorts() const;
		
		HttpServer(void);
		~HttpServer(void);
		HttpResponse processHttpRequest(HttpRequest &);
};

#endif

