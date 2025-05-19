#ifndef PLAYERSHIP_H
# define PLAYERSHIP_H

# include <stdbool.h>

# include "main.h"
# include "raylib.h"

# define BULLET_MAX 4

typedef struct s_PlayerShip
{
	t_Bullet	bulletPool[BULLET_MAX];
	Texture		texture;
	Rectangle	currSprite;	
	t_Circle	hitBox;
	Vector2		CoM;
	Vector2		pos;
	Vector2		vel;
	double		spawnTime;
	double		deathTime;
	double		hyperspaceTime;
	float		angle;
	float		accel;
	float		maxSpeed;
	bool		isLive;
	bool		inHyperspace;
}	t_PlayerShip;

void	decelerate(void);
void	accelerate(void);
void	playerShoot(void);
void	playerDie(void);
void	playerRespawn(void);
void	enterHyperspace(void);
void	exitHyperspace(void);
void	updatePlayer(void);
void	drawPlayer(void);
void	handlePlayerCollisions(void);
void	handlePlayerBulletCollisions(void);
void	updatePlayerBullets(void);
void	drawPlayer(void);
void	drawPlayerBullets(void);

extern t_PlayerShip	g_player;

#endif
