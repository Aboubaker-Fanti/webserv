NAME = webserv

CPPFLAGS = -Wall -Werror -Wextra -std=c++98 -fsanitize=address -g3
# -fsanitize=address -g3

CC = c++

SRC = main.cpp\
	Request.cpp\
	Response.cpp\
	function_utils.cpp\
	ChunkReader.cpp\
	ContentLengthRd.cpp\
	Location.cpp Server.cpp Webserve.cpp get.cpp\
	delete_methode.cpp\
	Cgi.cpp\
	ParseRequest.cpp\
	Post.cpp\
	
OBJDIR = obj

OBJ = ${SRC:.cpp=.o}

OBJS = $(addprefix $(OBJDIR)/,$(OBJ))

all: ${NAME}

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(OBJDIR)
	${CC} -fsanitize=address -g3 ${CPPFLAGS} -c $< -o $@

${NAME}: ${OBJS}
	${CC} -fsanitize=address -g3 ${SRC} -o ${NAME}

clean:
	rm -rf ${OBJS}

fclean: clean
	rm -rf ${NAME}

re: fclean all