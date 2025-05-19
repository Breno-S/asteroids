#ifndef SAUCER_H
# define SAUCER_H

# include <stdbool.h>

# include "main.h"
# include "raylib.h"

typedef struct s_Saucer
{
	t_Bullet	bullets[2];
	Texture		textureBg;
	Texture		textureSm;
	t_Circle	hitBox;
	Vector2		pos;
	Vector2		vel;
	double		spawnTime;
	double		deathTime;
	double		requestTime;
	double		shootTime;
	double		mourningTimer;
	float		speed;
	float		inaccuracy;
	enum e_Size	size;
	bool		shouldRespawn;
	bool		isLive;
}	t_Saucer;

void	saucerShoot(void);
void	saucerDie(void);
void	requestSaucer(void);
void	handleSaucerCollisions(void);
void	handleSaucerBulletCollisions(void);
void	updateSaucer(void);
void	updateSaucerBullets(void);
void	drawSaucer(void);
void	drawSaucerBullets(void);

extern t_Saucer	g_saucer;

#endif
