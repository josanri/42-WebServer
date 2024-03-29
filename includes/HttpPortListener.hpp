#ifndef HTTP_PORT_LISTENER_HPP
#define HTTP_PORT_LISTENER_HPP
# include <netinet/in.h>
# include "HttpRequest.hpp"
# include "HttpServer.hpp"
# include <set>
# include <map>
# include <vector>

class HttpPortListener
{
	private:
		int server_fd;
        int port;
		struct sockaddr_in address;
		std::set<int> openFileDescriptors;
		std::map<int, HttpPortListener*> & fileDescriptorToPort;
		std::map<int, HttpRequest> fileDescriptorToRequest;
		std::map<std::string, HttpServer *> serverNamesToServer;
		int bindServerConnection();
		void addConnection(int fd);
		void closeConnection(int fd);
		void sendResponse(const int & fd);
		void receiveRequest(const int & fd);
		void acceptConnection(const int & fd);
		int setTimeout(int fd);
		int setKeepAlive(int fd);
	public:
		HttpPortListener(int port, std::map<int, HttpPortListener*> & map, const std::vector<HttpServer *> & servers);
		HttpPortListener(const HttpPortListener &);
		~HttpPortListener(void);
		HttpPortListener & operator=(const HttpPortListener &);
		void initializeSocket();
		void initializeServerNamesToServer(const std::vector<HttpServer *> &servers);

		void connect(const int & fd, const int & revents);

		std::set<int> & getOpenFileDescriptors(void);
		int getServerFd() const;
		int getPort() const;
		std::vector<HttpServer *> getServer();

		void announce(std::ostream&) const;
};

std::ostream& operator<<(std::ostream &out, HttpPortListener const & rhs);
#endif