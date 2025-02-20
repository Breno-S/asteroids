NAME	=game
SRC		=main.c resources.c explosions.c gameObjects.c title.c game.c highScore.c postGame.c

CC		=cc
CFLAGS	=-Wall -Wextra -Werror
LDFLAGS	=-lraylib -lm

all: $(NAME)

run: $(NAME)
	./$(NAME)

$(NAME): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o $(NAME)

clean:
	rm -rf $(NAME)
