#ifndef GAMEOBJECTS_H
# define GAMEOBJECTS_H

# include "raylib.h"
# include "header.h"

# define BULLET_MAX 4

// GAMESTATE

typedef struct	s_GameState
{
	unsigned int		score;
	double				levelStartTime;
	double				cleanTime;
	unsigned char		numLives;
	unsigned short		oneUpMeter;
	unsigned short		rockCount;
	unsigned short		rocksToSpawn;
	enum e_GameScreen	currentScreen;
	bool				isPaused;
	bool				isGameOver;
}	t_GameState;

void			initSession();
void			endSession();
void			scoreAdd(unsigned short points);
void			drawLives();
void			drawScore();

// PLAYER

typedef struct	s_PlayerShip
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

void	decelerate();
void	accelerate();
void	playerShoot();
void	playerDie();
void	playerRespawn();
void	enterHyperspace();
void	exitHyperspace();
void	updatePlayer();
void	drawPlayer();
void	playerHandleInputs();
void	handlePlayerCollisions();
void	handlePlayerBulletCollisions();
void	updatePlayerBullets();
void	drawPlayer();
void	drawPlayerBullets();

// SAUCER

typedef struct	s_Saucer
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

void	saucerShoot();
void	saucerDie();
void	requestSaucer();
void	handleSaucerCollisions();
void	handleSaucerBulletCollisions();
void	updateSaucer();
void	updateSaucerBullets();
void	drawSaucer();
void	drawSaucerBullets();

// ROCKS

typedef struct	s_Rock
{
	t_Circle	hitBox;
	Vector2		pos;
	Vector2		vel;
	int			textureIdx;
	enum e_Size	size;
	bool		isLive;
}	t_Rock;

void	spawnRocks();
void	decayRock(unsigned short rockIdx, bool	shouldScore);
void	updateRocks();
void	drawRocks();

// RECORDS

typedef struct	s_Record
{
	char			initials[3];
	unsigned int	score;
}	t_Record;

unsigned short	loadRecords(t_Record *buffer);
void			saveRecord(char *initials, unsigned int score);
void			drawHighScore();

extern t_GameState	gameState;
extern t_PlayerShip	player;
extern t_Saucer		saucer;
extern t_Rock		rockPool[ROCK_MAX];
extern t_Record		localRecords[10];

#endif
