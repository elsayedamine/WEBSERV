NAME = webserv

CXX = c++
CXXFLAGS = -Wall -Wextra  -std=c++98 -g #-Werror 

SRCS =	main.cpp \
		utils/strings.cpp \
		utils/prints.cpp \
		Configuration/Directive.cpp \
		Configuration/Configuration.cpp \
		Configuration/ConfigBlock.cpp



OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
