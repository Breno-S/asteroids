#ifndef EXPLOSIONS_H
# define EXPLOSIONS_H

# include <stdbool.h>

# include "raylib.h"

# define EXPLOSION_DEBRIS 10

typedef struct s_Debris
{
	Vector2	pos;
	Vector2	vel;
}	t_Debris;

typedef struct s_Explosion
{
	t_Debris	debris[EXPLOSION_DEBRIS];
	double		startTime;
	bool		isLive;
}	t_Explosion;

void	explodeAt(Vector2 place);
void	updateExplosions(void);
void	drawExplosions(void);

#endif
