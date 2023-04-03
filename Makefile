NAME = webserv
SRC = main.cpp
OBJ = ${SRC:.cpp=.o}

CXX			=	c++
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++98 -g
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
	
.PHONY: all fclean clean re