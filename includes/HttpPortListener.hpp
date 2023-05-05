#ifndef HTTP_PORT_LISTENER_HPP
#define HTTP_PORT_LISTENER_HPP
# include <netinet/in.h>
# include "HttpServerConfiguration.hpp"
# include <set>
# include <vector>

class HttpPortListener
{
	private:
		int server_fd;
        int port;
		struct sockaddr_in address;
		std::set<int> openFileDescriptors;
		int prepareServerConnection();
	public:
		HttpPortListener(void);
		HttpPortListener(const HttpPortListener &);
		HttpPortListener(const HttpServerConfiguration &);
		~HttpPortListener(void);
		HttpPortListener & operator=(const HttpPortListener &);
		void initializeSocket();

		std::set<int> & getOpenFileDescriptors(void);
		int getServerFd() const;
		int getPort() const;

		void announce(std::ostream&) const;
};

std::ostream& operator<<(std::ostream &out, HttpPortListener const & rhs);
#endif