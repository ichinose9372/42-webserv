
NAME = webserv
CC = c++
CFLAGS = -std=c++98

SOURCES = main.cpp Request.cpp Response.cpp 

all : $(NAME)

$(NAME) : $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(NAME)

clean: 
	rm -f $(NAME) *.gch

fclean: clean

re: fclean all
