NAME = a.out
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98

SOURCES = main.cpp MainConfig.cpp
INCLUDE = MainConfig.hpp

all : $(NAME)

$(NAME) : $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) $(INCLUDE)

clean: 
	$(RM) $(NAME) *.gch

fclean:
	$(RM) $(NAME) *.gch

re: clean all