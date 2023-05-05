#ifndef HTTP_SERVER_CONFIGURATION_HPP
# define HTTP_SERVER_CONFIGURATION_HPP

#include <string>

class HttpServerConfiguration
{
	private:
        std::string host;
		int number_of_conections;
		int port;
	public:
		int getNumberOfConections() const;
		int getPort() const;
		
		HttpServerConfiguration(void);
		~HttpServerConfiguration(void);
};

#endif

