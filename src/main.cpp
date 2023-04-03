#include <iostream>

int server_daemon();

void fake_read_configuration_file(char const *conf_file)
{
	(void) conf_file;
}

int main(int argc, char const **argv)
{
	if (argc != 2) {
		std::cerr << "usage: ./webserv configuration_file" << std::endl;
		return (1);
	}
	fake_read_configuration_file(argv[1]);
	server_daemon();
	return 0;
}
