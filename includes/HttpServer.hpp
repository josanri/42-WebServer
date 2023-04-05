#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP
# include <netinet/in.h>
# include "HttpServerConfiguration.hpp"
# include <set>

class HttpServer
{
	private:
		int server_fd;
		struct sockaddr_in address;
		HttpServerConfiguration configuration;
		std::set<int> openFileDescriptors;
		void initializeSocket();
		int prepareServerConnection();
	public:
		std::set<int> & getOpenFileDescriptors(void);

		HttpServer(void);
		HttpServer(const HttpServer &);
		HttpServer(const HttpServerConfiguration &);
		~HttpServer(void);
		HttpServer & operator=(const HttpServer &);

		int getServerFd() const;
		void announce(std::ostream&) const;
};

std::ostream& operator<<(std::ostream &out, HttpServer const & rhs);
#endif