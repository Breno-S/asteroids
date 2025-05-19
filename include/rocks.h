#ifndef ROCKS_H
# define ROCKS_H

# include <stdbool.h>

# define ROCK_SPAWN_MAX 11
# define ROCK_MAX 44

typedef struct s_Rock
{
	t_Circle	hitBox;
	Vector2		pos;
	Vector2		vel;
	int			textureIdx;
	enum e_Size	size;
	bool		isLive;
}	t_Rock;

void	spawnRocks(void);
void	decayRock(unsigned short rockIdx, bool	shouldScore);
void	updateRocks(void);
void	drawRocks(void);

extern t_Rock	g_rocks[ROCK_MAX];

#endif
