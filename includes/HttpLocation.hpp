#ifndef HTTP_LOCATION_HPP
# define HTTP_LOCATION_HPP

# include <string>
# include <map>
# include <vector>

class HttpLocation
{
	private:
		std::string root;
		std::string route;
		std::vector<std::string> methods;
		std::string defaultFile;
		std::string redirectionRoute;
		std::map<std::string, std::string> fileExtensionToCGI;
		bool directoryListing;
		bool upload;
		unsigned int maxBodySize;

	public:
		HttpLocation(void);
		HttpLocation(std::string root, std::string route, std::vector<std::string> & methods, std::string defaultFile, std::string redirectionRoute, std::map<std::string, std::string> & fileExtensionToCGI, bool directoryListing, bool upload, unsigned int maxBodySize);
		~HttpLocation(void);
		bool getUpload() const;
		std::string getRoute() const;
		std::string getRoot() const;
		std::string getCgi(std::string extension) const;
		std::string getDefaultFile() const;
		std::string getRedirectionRoute() const;
		bool isMethodAllowed(std::string method);
		bool directoryListingAllowed() const;
		unsigned int getMaxBodySize() const;

};

#endif

