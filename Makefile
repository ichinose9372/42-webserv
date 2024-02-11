NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
SUBDIRS = cnf http signal socket
OBJDIR = objs
RM = rm -rf

# ソースファイルの検索
SRCS = $(shell find $(SUBDIRS) -type f -name "*.cpp")
# オブジェクトファイルのパスの生成
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS) main.cpp
	$(CXX) $(CXXFLAGS) $(OBJS) main.cpp -o $(NAME)
	python3 -m venv venv
	. venv/bin/activate && pip install --upgrade pip && pip install flask
#	source venv/bin/activate

# オブジェクトファイルのコンパイルルール
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJDIR)

fclean: clean
	$(RM) $(NAME)
	$(RM) venv
	$(RM) ./docs/upload/post*
	$(RM) ./docs/upload/test*
	$(RM) test.txt

re: fclean all

.PHONY: all clean fclean re
