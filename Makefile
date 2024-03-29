NAME = webserv
SRC = $(addprefix src/,main.cpp HttpServer.cpp HttpLocation.cpp HttpPortListener.cpp HttpRequest.cpp HttpResponse.cpp Parser.cpp Extractor.cpp utils.cpp)
OBJ = ${SRC:.cpp=.o}

CXX			=	c++
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98 -g -I includes
RM			=	rm -f

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

run: $(NAME)
	@./$(NAME)

test: $(NAME)
	@./$(NAME) testers/tester.cfg

.PHONY: all fclean clean re