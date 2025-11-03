NAME = webserv

CXX = c++
CXXFLAGS = -Wall -Wextra  -std=c++98 #-Werror

SRCS = main.cpp Directive.cpp Configuration.cpp utils/prints.cpp utils/strings.cpp validate_directive.cpp


OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
