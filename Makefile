CXX:= c++
CFLAGS:= -Wall -Wextra -Werror -std=c++98 -g -fpermissive
NAME:= webserv
INC= -I./includes
BIN_DIR:= bin
SRC:= src/Config/ConfigBlock.cpp \
	src/Config/Directive.cpp \
	src/Config/Configuration.cpp \
	src/Methods/CGI.cpp \
	src/Methods/Post.cpp \
	src/Methods/Get.cpp \
	src/Methods/Utils.cpp \
	src/Processing/Connection.cpp \
	src/Processing/PreProcess.cpp \
	src/Processing/Request.cpp \
	src/Processing/Compare.cpp \
	src/Processing/Parse.cpp \
	src/Processing/Process.cpp \
	src/Processing/Response.cpp \
	src/Server/Server.cpp \
	src/utils/prints.cpp \
	src/utils/strings.cpp \
	src/main.cpp

OBJ:= $(patsubst %.cpp, $(BIN_DIR)/%.o, $(notdir $(SRC)))

vpath %.cpp $(dir $(SRC))

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CFLAGS) $(INC) -o $@ $(OBJ)

$(BIN_DIR)/%.o: %.cpp $(INCLUDES) | $(BIN_DIR)
	$(CXX) $(CFLAGS) $(INC) -o $@ -c $<

clean:
	rm -rf $(BIN_DIR)

fclean: 
	rm -rf $(BIN_DIR)
	rm -rf $(NAME)

re: fclean $(NAME)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

.PHONY: all clean fclean re
.SECONDARY: