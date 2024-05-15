CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++20 -g
NAME = ircserv
CPPFILES = \
			src/Client.cpp \
			src/Channel.cpp \
			src/Server.cpp \
			src/utils.cpp \
			src/Executor.cpp \
			src/main.cpp

OFILES = $(CPPFILES:.cpp=.o)

all: $(NAME)

$(NAME): $(OFILES)
	$(CC) $(CFLAGS) $(OFILES) $(LIB) -o $(NAME)

%.o: %.cpp
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo COMPILED: $<

re: fclean all

fclean: clean
	@rm -f $(NAME)
	@echo "DEEP CLEANING"

clean:
	@rm -f $(OFILES)
	@echo "CLEANED UP"

.PHONY: clean re fclean all
