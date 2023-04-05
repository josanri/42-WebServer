#ifndef HTTP_SERVER_CONFIGURATION_HPP
# define HTTP_SERVER_CONFIGURATION_HPP

class HttpServerConfiguration
{
	private:
		int number_of_conections;
		int port;
	public:
		int getNumberOfConections() const;
		int getPort() const;
		
		HttpServerConfiguration(void);
		~HttpServerConfiguration(void);
};

#endif

