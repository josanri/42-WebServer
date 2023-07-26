#ifndef PARSER_HPP
# define PARSER_HPP

# include <string>
# include <vector>
# include "HttpServer.hpp"
# include "HttpPortListener.hpp"

class Parser {
	private:
		std::string filePath;
		std::string fileContent;

		void readFile();
		std::string extractChunk(std::string  & content, std::string const & chunkName);
		HttpServer * extractServer();
		HttpLocation * extractLocation(std::string & serverChunk);


	public:
		Parser(std::string const & filename);
		~Parser();

		// std::vector<HttpServer *> parse(void);
		void parse(std::vector<HttpPortListener *> & listeners, std::map<int,HttpPortListener *> & fileDescriptoToPort);
};

#endif
