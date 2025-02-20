#ifndef HEADER_H
# define HEADER_H

#include "raylib.h"

/*********************************** MACROS ***********************************/

#define SC_W 640
#define SC_H 480

#define ROCK_MAX 44
#define ROCK_SPAWN_MAX 11
#define SAVEFILE "./data"

/*********************************** ENUMS ************************************/

enum e_Size { BIG, MEDIUM, SMALL };
enum e_GameScreen { TITLE, GAME, HIGHSCORE, POSTGAME };

/*********************************** TYPES ************************************/

typedef struct	s_Circle
{
	Vector2	center;
	float	radius;
}	t_Circle;

typedef struct	s_Bullet
{
	Vector2	pos;
	Vector2	vel;
	float	ttl;
	bool	isLive;
}	t_Bullet;

#endif
