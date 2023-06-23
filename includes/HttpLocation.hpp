#ifndef HTTP_LOCATION_HPP
# define HTTP_LOCATION_HPP

# include <string>
# include <map>
# include <vector>

class HttpLocation
{
	private:
		std::vector<std::string> methods;
		std::string defaultFile;
		std::string redirection;
		std::map<std::string, std::string> fileExtensionToCGI;
		bool directoryListing;
		bool upload;
	public:
		HttpLocation(void);
		~HttpLocation(void);

};

#endif

