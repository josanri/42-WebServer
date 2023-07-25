#ifndef HTTP_LOCATION_HPP
# define HTTP_LOCATION_HPP

# include <string>
# include <map>
# include <vector>

class HttpLocation
{
	private:
		std::string route;
		std::vector<std::string> methods;
		std::string defaultFile;
		std::string redirectionRoute;
		std::map<std::string, std::string> fileExtensionToCGI;
		bool directoryListing;
		bool upload;

	public:
		HttpLocation(void);
		HttpLocation(std::string route, std::vector<std::string> & methods, std::string defaultFile, std::string redirectionRoute, std::map<std::string, std::string> & fileExtensionToCGI, bool directoryListing, bool upload);
		~HttpLocation(void);
		std::string getRoute() const;
		bool isMethodAllowed(std::string method);

};

#endif

