NAME= req
CXX= c++
FLAGS= -Wall -Wextra -Werror -std=c++98 -g
SRC= Parse.cpp main.cpp Request.cpp
OBJ= $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(OBJ) -o $(NAME)

%.opp: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all