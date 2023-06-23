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
		std::vector<int> ports;
		std::vector<std::string> serverNames;
		std::vector<HttpLocation *> locations;
		std::map<int, std::string> errorNumberToLocation;
		unsigned int maxBody;
	public:
		
		HttpServer(void);
		HttpServer(std::vector<int> & ports, std::vector<std::string> &  serverNames, std::vector<HttpLocation *> & locations, std::map<int, std::string> & errorNumberToLocation, unsigned int maxBody);
		~HttpServer(void);
		HttpResponse processHttpRequest(HttpRequest &);
		const std::vector<std::string> & getServerNames() const;
		const std::vector<int> & getPorts() const;

};

#endif

