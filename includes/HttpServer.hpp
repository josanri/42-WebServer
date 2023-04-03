#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP
#include <netinet/in.h>

class HttpServer
{
	private:
		int server_fd;
		int number_of_conections;
		int port;
		struct sockaddr_in address;
		int prepare_server_connection();
	public:
		HttpServer(void);
		HttpServer(const HttpServer &);
		~HttpServer(void);
		HttpServer & operator=(const HttpServer &);
};

#endif