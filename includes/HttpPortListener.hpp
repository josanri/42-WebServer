#ifndef HTTP_PORT_LISTENER_HPP
#define HTTP_PORT_LISTENER_HPP
# include <netinet/in.h>
# include "HttpServerConfiguration.hpp"
# include <set>
# include <map>
# include <vector>

#define BUFFER_SIZE 8192
#define LISTEN_BACKLOG 20

class HttpPortListener
{
	private:
		int server_fd;
        int port;
		struct sockaddr_in address;
		std::set<int> openFileDescriptors;
		std::map<int, HttpPortListener*> & fileDescriptorToPort;
		int bindServerConnection();
		void addConnection(int fd);
		void closeConnection(int fd);
	public:
		HttpPortListener(std::map<int, HttpPortListener*> &);
		HttpPortListener(const HttpPortListener &);
		HttpPortListener(const HttpServerConfiguration &);
		~HttpPortListener(void);
		HttpPortListener & operator=(const HttpPortListener &);
		void initializeSocket();
		void connect(const int & fd, const int & revents);

		std::set<int> & getOpenFileDescriptors(void);
		int getServerFd() const;
		int getPort() const;

		void announce(std::ostream&) const;
};

std::ostream& operator<<(std::ostream &out, HttpPortListener const & rhs);
#endif