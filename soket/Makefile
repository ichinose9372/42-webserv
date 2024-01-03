CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
SRCS = Server.cpp Timer.cpp 
OBJS = $(SRCS:.cpp=.o)
OBJDIR = objs

.PHONY: all clean

all: $(addprefix $(OBJDIR)/,$(OBJS))

$(OBJDIR)/%.o: %.cpp
	mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)