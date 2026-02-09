# check (bdl hdch)
CXX:= g++
CFLAGS:= -std=c++98 -g -Wall -Werror -Wextra
NAME:= webserv
INC= -I./includes
BIN_DIR:= bin
SRC:= src/Config/ConfigBlock.cpp \
	src/Config/Directive.cpp \
	src/Config/Configuration.cpp \
	src/Methods/CGI.cpp \
	src/Methods/Post.cpp \
	src/Methods/Get.cpp \
	src/Methods/Delete.cpp \
	src/Methods/Put.cpp \
	src/Methods/Utils.cpp \
	src/Server/Connection.cpp \
	src/Server/PreProcess.cpp \
	src/Server/Request.cpp \
	src/Server/Parser.cpp \
	src/Server/Process.cpp \
	src/Server/Response.cpp \
	src/Server/Server.cpp \
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