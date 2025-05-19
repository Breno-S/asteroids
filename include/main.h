#ifndef MAIN_H
# define MAIN_H

# include <stdbool.h>

# include "raylib.h"

# define SC_W 640
# define SC_H 480

# define SAVEFILE "./records.dat"

enum e_Size
{
	BIG,
	MEDIUM,
	SMALL
};

enum e_GameScreen
{
	TITLE,
	GAME,
	HIGHSCORE,
	POSTGAME
};

typedef struct s_Circle
{
	Vector2	center;
	float	radius;
}	t_Circle;

typedef struct s_Bullet
{
	Vector2	pos;
	Vector2	vel;
	float	ttl;
	bool	isLive;
}	t_Bullet;

extern double	g_time;
extern float	g_frameTime;

#endif
