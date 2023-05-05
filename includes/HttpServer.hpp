#ifndef HTTP_SERVER_HPP
# define HTTP_SERVER_HPP

#include <vector>
#include <string>

class HttpServerConHttpServerfiguration
{
	private:
        std::string host;
		std::vector<int> ports;
	public:
		int getNumberOfConections() const;
		int getPort() const;
		
		HttpServerConHttpServerfiguration(void);
		~HttpServerConHttpServerfiguration(void);
};

#endif

