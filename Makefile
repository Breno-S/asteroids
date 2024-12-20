NAME	= game
SRC		= main.c

CC		= cc
CFLAGS	= -Wall -Wextra #-Werror
LDFLAGS	= -lraylib -lm

all: $(NAME)

run: $(NAME)
	./$(NAME)

$(NAME): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o $(NAME)

clean:
	rm -rf $(NAME)
