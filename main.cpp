#include <iostream>

int main(int argc, char const **argv)
{
	if (argc != 2) {
		std::cerr << "usage: ./webserv configuration_file" << std::endl;
		return (1);
	}

	(void) argv;
	return 0;
}
