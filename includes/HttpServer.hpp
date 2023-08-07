#ifndef HTTP_SERVER_HPP
# define HTTP_SERVER_HPP

# include <vector>
# include <string>
# include <fstream>
# include <sys/stat.h>
# include <dirent.h>
# include <sstream>

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

		HttpResponse handle_get(HttpRequest & request, HttpLocation *location);
		HttpResponse handle_post(HttpRequest & request, HttpLocation *location);
		HttpResponse handle_put(HttpRequest & request, HttpLocation *location);
		HttpResponse handle_delete(HttpRequest & request, HttpLocation *location);
		HttpLocation* getLocation(HttpRequest & request);
		HttpResponse cgi (HttpRequest & request, std::string & path, std::string & cgiPath);

		bool exists(const std::string & path);
		bool isDirectory(const std::string & path);
		std::string listDirectory(const std::string & path, const std::string & requestRoute);

	public:
		
		HttpServer(void);
		HttpServer(std::vector<int> & ports, std::vector<std::string> &  serverNames, std::vector<HttpLocation *> & locations, std::map<int, std::string> & errorNumberToLocation);
		HttpServer & operator=(const HttpServer & rhs);
		~HttpServer(void);
		HttpResponse processHttpRequest(HttpRequest &);
		const std::vector<std::string> & getServerNames() const;
		const std::vector<int> & getPorts() const;
		std::vector<HttpLocation *> getLocations();
		const std::map<int, std::string> & getErrorNumberToLocation() const;


};

#endif

