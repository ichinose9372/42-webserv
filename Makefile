NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
SUBDIRS = cnf http soket

RM = rm -rf

all: $(SUBDIRS)
	$(eval OBJS = $(foreach dir, $(SUBDIRS), $(wildcard $(dir)/objs/*.o)))
	$(CXX) $(CXXFLAGS) $(OBJS) main.cpp -o $(NAME)

$(SUBDIRS):
	$(MAKE) -C $@

clean:
	for dir in $(SUBDIRS); do \
        $(MAKE) -C $$dir clean; \
    done
	$(RM) main.o

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: $(SUBDIRS) all clean fclean re
