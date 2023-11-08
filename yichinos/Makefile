NAME = a.out
CC = c++
CFLAGS = -std=c++98

SOURCES = main.cpp MainConfig.cpp Servers.cpp Locations.cpp ExclusivePath.cpp
INCLUDE = MainConfig.hpp Servers.hpp Locations.hpp ExclusivePath.hpp

all : $(NAME)

$(NAME) : $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) $(INCLUDE)

clean: 
	$(RM) $(NAME) *.gch

fclean:
	$(RM) $(NAME) *.gch

re: clean all