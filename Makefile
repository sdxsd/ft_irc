CC = g++
CFLAGS = -g -Wall -Wextra -Werror -std=c++23
NAME = ircserv
CPPFILES = \
			src/Client.cpp \
			src/Server.cpp \
			src/main.cpp
OFILES = $(CPPFILES:.c=.o)

all: $(NAME)

$(NAME): $(OFILES)
	$(CC) $(CFLAGS) $(OFILES) $(LIB) -o $(NAME)

%.o: %.c
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
