#ifndef HTTP_SERVER_HPP
# define HTTP_SERVER_HPP

# include <string>
# include <map>
# include <vector>


class HttpLocation
{
	private:
        std::string host;
		std::vector<int> ports;
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

