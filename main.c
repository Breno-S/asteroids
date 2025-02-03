#include "raylib.h"
#include "raymath.h"
#include "explosions.h"
#include <stdio.h>

/*********************************** MACROS ***********************************/

#define SC_W 640
#define SC_H 480

#define BULLET_MAX 4
#define ROCK_MAX 64
#define ROCK_SPAWN_MAX 11

/*********************************** ENUMS ************************************/

enum e_Size { BIG, MEDIUM, SMALL };
enum e_GameScreen { TITLE, GAME, END };

/*********************************** TYPES ************************************/

typedef struct	s_GameState
{
	unsigned long		score;
	double				deathTime;
	double				saucerDeathTime;
	double				cleanTime;
	double				hyperspaceTime;
	double				levelStartTime;
	unsigned short		numLives;
	unsigned short		oneUpMeter;
	unsigned short		rockCount;
	unsigned short		rocksToSpawn;
	enum e_GameScreen	currentScreen;
	bool				isPaused;
}	t_GameState;

typedef struct	s_Circle
{
	Vector2	center;
	float	radius;
}	t_Circle;

typedef struct	s_Rock
{
	t_Circle	hitBox;
	Vector2		pos;
	Vector2		vel;
	int			textureIdx;
	enum e_Size	size;
	bool		isLive;
}	t_Rock;

typedef struct	s_Bullet
{
	Vector2	pos;
	Vector2	vel;
	float	ttl;
	bool	isLive;
}	t_Bullet;

typedef struct	s_PlayerShip
{
	Texture		texture;
	Rectangle	currSprite;	
	t_Circle	hitBox;
	Vector2		CoM;
	Vector2		pos;
	Vector2		vel;
	float		angle;
	float		accel;
	float		maxSpeed;
	bool		isLive;
	bool		inHyperspace;
}	t_PlayerShip;

typedef struct	s_Saucer
{
	t_Bullet	bullets[2];
	Texture		texture;
	t_Circle	hitBox;
	Vector2		pos;
	Vector2		vel;
	double		spawnTime;
	double		shootTime;
	float		speed;
	float		inaccuracy;
	enum e_Size	size;
	bool		isLive;
}	t_Saucer;

/***************************** GLOBAL VARIABLES *******************************/

Texture			rockTextures[12];
t_GameState		gameState = {
	.numLives = 3,
	.currentScreen = TITLE,
	.rocksToSpawn = 4,
};
t_PlayerShip	player = {
	.currSprite = {0, 0, 21, 10},
	.hitBox = {{SC_W / 2, SC_H / 2}, 5},
	.CoM = {10, 5},
	.pos = {SC_W / 2, SC_H / 2},
	.maxSpeed = 250,
	.accel = 150,
};
t_Saucer	saucer = {
	.speed = 70,
	.inaccuracy = 20 * DEG2RAD
};
float			frameTime;
t_Bullet		bulletPool[BULLET_MAX];
t_Rock			rockPool[ROCK_MAX];

/********************************* FUNCTIONS **********************************/

void	decelerate()
{
	if (Vector2Length(player.vel) > 0)
		player.vel = Vector2Subtract(player.vel, Vector2Scale(player.vel, 0.25 * frameTime));
	player.currSprite.x = 0;
}

void	accelerate()
{
	float	speed;
	float	scale;
	
	player.vel.x += player.accel * cosf(player.angle * DEG2RAD) * frameTime;
	player.vel.y += player.accel * sinf(player.angle * DEG2RAD) * frameTime;
	speed = Vector2Length((Vector2){player.vel.x, player.vel.y});
	if (speed > player.maxSpeed)
	{
		scale = player.maxSpeed / speed;
		player.vel.x *= scale;
		player.vel.y *= scale;
	}
	player.currSprite.x = 21;
}

void	playerShoot()
{
	static unsigned	short	bulletIdx = 0;
	bool					isFull = true;
	float					bulletSpeed = player.maxSpeed;

	for (int i = 0; i < BULLET_MAX; i++)
		if (bulletPool[i].isLive == false)
			isFull = false;
	if (!isFull)
	{
		bulletPool[bulletIdx].pos = (Vector2){
			.x = player.pos.x + 8 * cosf(player.angle * DEG2RAD),
			.y = player.pos.y + 8 * sinf(player.angle * DEG2RAD)
		};
		bulletPool[bulletIdx].vel = (Vector2){
			.x = bulletSpeed * cosf(player.angle *DEG2RAD) + player.vel.x,
			.y = bulletSpeed * sinf(player.angle *DEG2RAD) + player.vel.y
		};
		bulletPool[bulletIdx].ttl = 1.3;
		bulletPool[bulletIdx].isLive = true;
		bulletIdx++;
		if (bulletIdx == BULLET_MAX)
			bulletIdx = 0;
	}
}

void	saucerShoot()
{
	static bool		bulletIdx = 0;
	float			bulletSpeed = player.maxSpeed * 1.25;
	float			angle;

	if (!saucer.bullets[bulletIdx].isLive)
	{
		switch (saucer.size)
		{
			case BIG:
				angle = GetRandomValue(0, 360) * DEG2RAD;
				break;
			case SMALL:
				angle = Vector2LineAngle(saucer.hitBox.center, player.pos) + saucer.inaccuracy;
				break;
			default:
		}
		saucer.bullets[bulletIdx].pos = (Vector2){
			.x = saucer.hitBox.center.x + 5 * cosf(angle),
			.y = saucer.hitBox.center.y + 5 * sinf(angle)
		};
		saucer.bullets[bulletIdx].vel = (Vector2){
			.x = bulletSpeed * cosf(angle),
			.y = bulletSpeed * -sinf(angle)
		};
		saucer.bullets[bulletIdx].ttl = 1;
		saucer.bullets[bulletIdx].isLive = true;
		bulletIdx = !bulletIdx;
		saucer.shootTime = GetTime();
	}
	if (GetRandomValue(0, 1))
		saucer.inaccuracy *= -1;
}

void	enterHyperspace()
{
	gameState.hyperspaceTime = GetTime();
	player.inHyperspace = true;
}

void	exitHyperspace()
{
	if (GetTime() - gameState.hyperspaceTime > 1.5)
	{
		player.vel.x = 0;
		player.vel.y = 0;
		player.pos.x = GetRandomValue(40, 600);
		player.pos.y = GetRandomValue(40, 440);
		player.hitBox.center.x = player.pos.x;
		player.hitBox.center.y = player.pos.y;
		player.inHyperspace = false;
	}
}

void	playerDie()
{
		explodeAt(player.hitBox.center);
		player.isLive = false;
		player.vel.x = 0;
		player.vel.y = 0;
		player.pos.x = SC_W / 2;
		player.pos.y = SC_H / 2;
		player.hitBox.center.x = SC_W / 2;
		player.hitBox.center.y = SC_H / 2;
		gameState.numLives--;
		gameState.deathTime = GetTime();
		gameState.saucerDeathTime = GetTime();
}

void	saucerDie()
{
	explodeAt(saucer.hitBox.center);
	saucer.isLive = false;
	if (saucer.size == SMALL && saucer.inaccuracy > 0)
		saucer.inaccuracy -= 1 * DEG2RAD;
	gameState.saucerDeathTime = GetTime();
}

void	playerRespawn()
{
	for (int i = 0; i < ROCK_MAX; i++)
	{
		if (rockPool[i].isLive)
		{
			if (CheckCollisionCircleRec(
						rockPool[i].hitBox.center,
						rockPool[i].hitBox.radius,
						(Rectangle){SC_W/2 - 50, SC_H/2 - 50, 100, 100})
			) {
				return;
			}
		}
	}
	if (GetTime() - gameState.deathTime > 3)
	{
		saucer.bullets[0].isLive = false;
		saucer.bullets[1].isLive = false;
		saucer.isLive = false;
		player.isLive = true;
	}
}

void	updatePlayer()
{
	player.pos.x += player.vel.x * frameTime;
	player.pos.y += player.vel.y * frameTime;
	player.hitBox.center.x += player.vel.x * frameTime;
	player.hitBox.center.y += player.vel.y * frameTime;
	if (player.pos.x > SC_W)
	{
		player.pos.x = 0;
		player.hitBox.center.x = 0;
	}
	if (player.pos.y > SC_H)
	{
		player.pos.y = 0;
		player.hitBox.center.y = 0;
	}
	if (player.pos.x < 0)
	{
		player.pos.x = SC_W;
		player.hitBox.center.x = SC_W;
	}
	if (player.pos.y < 0)
	{
		player.pos.y = SC_H;
		player.hitBox.center.y = SC_H;
	}
}

void	updateSaucer()
{
	short angles[3] = {-35, 0, 35};
	static struct {unsigned n: 2;} idx = {};

	if (saucer.isLive)
	{
		saucer.pos = Vector2Add(saucer.pos, Vector2Scale(saucer.vel, frameTime));
		saucer.hitBox.center = Vector2Add(saucer.hitBox.center, Vector2Scale(saucer.vel, frameTime));
		if (GetTime() - saucer.spawnTime > 2)
		{
			if (saucer.vel.x > 0)
			{
				if (idx.n == 0)
					saucer.vel = Vector2Rotate((Vector2){saucer.speed, 0}, angles[GetRandomValue(1, 2)] * DEG2RAD);
				else if (idx.n == 1)
					saucer.vel = Vector2Rotate((Vector2){saucer.speed, 0}, angles[GetRandomValue(0, 2)] * DEG2RAD);
				else
					saucer.vel = Vector2Rotate((Vector2){saucer.speed, 0}, angles[GetRandomValue(0, 1)] * DEG2RAD);
			}
			else
			{
				if (idx.n == 0)
					saucer.vel = Vector2Rotate((Vector2){saucer.speed, 0}, (angles[GetRandomValue(1, 2)] + 180) * DEG2RAD);
				else if (idx.n == 1)
					saucer.vel = Vector2Rotate((Vector2){saucer.speed, 0}, (angles[GetRandomValue(0, 2)] + 180) * DEG2RAD);
				else
					saucer.vel = Vector2Rotate((Vector2){saucer.speed, 0}, (angles[GetRandomValue(0, 1)] + 180) * DEG2RAD);
			}
			idx.n++;
			saucer.spawnTime = GetTime();
		}
		if (saucer.hitBox.center.y > SC_H)
		{
			saucer.hitBox.center.y = 0;
			saucer.pos.y = (saucer.size == BIG) ? -8 : -4;
		}
		if (saucer.hitBox.center.y < 0)
		{
			saucer.hitBox.center.y = SC_H;
			saucer.pos.y = (saucer.size == BIG) ? SC_H - 8 : SC_H - 4;
		}
		if (saucer.pos.x > SC_W + 1 || saucer.pos.x < -25)
		{
			saucer.isLive = false;
			gameState.saucerDeathTime = GetTime();
		}
		if ((GetTime() - saucer.shootTime > 0.66 && GetTime() - saucer.spawnTime > 1)
				&& saucer.hitBox.center.x < SC_W - saucer.hitBox.radius*2
				&& saucer.hitBox.center.x > saucer.hitBox.radius*2
				&& player.isLive)
			saucerShoot();
	}
}

void	updateRocks()
{
	for (int i = 0; i < ROCK_MAX; i++)
	{
		rockPool[i].pos = Vector2Add(rockPool[i].pos, Vector2Scale(rockPool[i].vel, frameTime));
		rockPool[i].hitBox.center = Vector2Add(rockPool[i].hitBox.center, Vector2Scale(rockPool[i].vel, frameTime));
		if (rockPool[i].hitBox.center.x > SC_W)
		{
			rockPool[i].pos.x = 0 - rockPool[i].hitBox.radius;
			rockPool[i].hitBox.center.x = 0;
		}
		if (rockPool[i].hitBox.center.x < 0)
		{
			rockPool[i].pos.x = SC_W - rockPool[i].hitBox.radius;
			rockPool[i].hitBox.center.x = SC_W;
		}
		if (rockPool[i].hitBox.center.y > SC_H)
		{
			rockPool[i].pos.y = 0 - rockPool[i].hitBox.radius;
			rockPool[i].hitBox.center.y = 0;
		}
		if (rockPool[i].hitBox.center.y < 0)
		{
			rockPool[i].pos.y = SC_H - rockPool[i].hitBox.radius;
			rockPool[i].hitBox.center.y = SC_H;
		}
	}
}

void	updateBullets()
{
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (bulletPool[i].isLive)
		{
			bulletPool[i].pos = Vector2Add(bulletPool[i].pos, Vector2Scale(bulletPool[i].vel, frameTime));
			if (bulletPool[i].pos.x > SC_W) bulletPool[i].pos.x = 0;
			if (bulletPool[i].pos.y > SC_H) bulletPool[i].pos.y = 0;
			if (bulletPool[i].pos.x < 0)    bulletPool[i].pos.x = SC_W;
			if (bulletPool[i].pos.y < 0)    bulletPool[i].pos.y = SC_H;
			bulletPool[i].ttl -= frameTime;
			if (bulletPool[i].ttl <= 0.0F)
				bulletPool[i].isLive = false;
		}
	}
	if (saucer.bullets[0].isLive)
	{
		saucer.bullets[0].pos = Vector2Add(saucer.bullets[0].pos, Vector2Scale(saucer.bullets[0].vel, frameTime));
		saucer.bullets[0].ttl -= frameTime;
		if (saucer.bullets[0].ttl <= 0.0F)
			saucer.bullets[0].isLive = false;
	}
	if (saucer.bullets[1].isLive)
	{
		saucer.bullets[1].pos = Vector2Add(saucer.bullets[1].pos, Vector2Scale(saucer.bullets[1].vel, frameTime));
		saucer.bullets[1].ttl -= frameTime;
		if (saucer.bullets[1].ttl <= 0.0F)
			saucer.bullets[1].isLive = false;
	}
}

void	loadAllTextures()
{
	rockTextures[0] = LoadTexture("resources/Rock1-bg.png");
	rockTextures[1] = LoadTexture("resources/Rock2-bg.png");
	rockTextures[2] = LoadTexture("resources/Rock3-bg.png");
	rockTextures[3] = LoadTexture("resources/Rock4-bg.png");

	rockTextures[4] = LoadTexture("resources/Rock1-md.png");
	rockTextures[5] = LoadTexture("resources/Rock2-md.png");
	rockTextures[6] = LoadTexture("resources/Rock3-md.png");
	rockTextures[7] = LoadTexture("resources/Rock4-md.png");

	rockTextures[8] = LoadTexture("resources/Rock1-sm.png");
	rockTextures[9] = LoadTexture("resources/Rock2-sm.png");
	rockTextures[10] = LoadTexture("resources/Rock3-sm.png");
	rockTextures[11] = LoadTexture("resources/Rock4-sm.png");

	player.texture = LoadTexture("resources/Ship.png");
}

void	drawPlayer()
{
	if (player.isLive && !player.inHyperspace)
	{
		DrawTexturePro(
			player.texture,
			player.currSprite,
			(Rectangle){player.pos.x, player.pos.y, player.currSprite.width, player.currSprite.height},
			player.CoM,
			player.angle,
			WHITE
		);
		DrawCircleLinesV(player.hitBox.center, player.hitBox.radius, RED);
	}
}

void	drawSaucer()
{
	if (saucer.isLive)
	{
		DrawTexture(
			saucer.texture,
			saucer.pos.x,
			saucer.pos.y,
			WHITE
		);
		DrawCircleLinesV(saucer.hitBox.center, saucer.hitBox.radius, RED);
	}
}

void	drawBullets()
{
	for (int i = 0; i < BULLET_MAX; i++)
		if (bulletPool[i].isLive)
		{
			DrawPixel(bulletPool[i].pos.x, bulletPool[i].pos.y, WHITE);
			DrawPixel(bulletPool[i].pos.x+1, bulletPool[i].pos.y, WHITE);
			DrawPixel(bulletPool[i].pos.x+1, bulletPool[i].pos.y+1, WHITE);
			DrawPixel(bulletPool[i].pos.x, bulletPool[i].pos.y+1, WHITE);
		}
	if (saucer.bullets[0].isLive)
		DrawPixel(saucer.bullets[0].pos.x, saucer.bullets[0].pos.y, WHITE);
	if (saucer.bullets[1].isLive)
		DrawPixel(saucer.bullets[1].pos.x, saucer.bullets[1].pos.y, WHITE);
}

void	drawRocks()
{
	for (int i = 0; i < ROCK_MAX; i++)
	{
		if (rockPool[i].isLive)
		{
			DrawTexture(
				rockTextures[rockPool[i].textureIdx + 4 * rockPool[i].size],
				rockPool[i].pos.x,
				rockPool[i].pos.y,
				WHITE
			);
			//char	stats[32] = { 0 };
			DrawCircleLinesV(rockPool[i].hitBox.center, rockPool[i].hitBox.radius, RED);
			//sprintf(stats, "%.2f", Vector2Length(rockPool[i].vel)); 
			//DrawText(stats, rockPool[i].pos.x, rockPool[i].pos.y + 30, 10, YELLOW);
		}
	}
}

void	drawScore()
{
	static char	scoreStr[15];

	sprintf(scoreStr, "%12.2ld", gameState.score);
	DrawText(scoreStr, 0, 4, 26, (Color){255, 255, 255, 127});
}

void	drawLives()
{
	for (int i = 0; i < gameState.numLives; i++)
	{
		DrawTexturePro(
			player.texture,
			(Rectangle){0, 0, 21, 10},
			(Rectangle){90 + 10 * i, 42, player.currSprite.width, player.currSprite.height},
			player.CoM,
			-90,
			(Color){255, 255, 255, 127}
		);
	}
}
	
void	drawTitleScreen()
{
	static unsigned char	alpha = 40;
	static unsigned char	dAlpha = 8;
	DrawText("ASTEROIDS", 35, SC_H/2 - 48, 96, (Color){255, 255, 255, 255});
	DrawText("Press ENTER to play", SC_W/2 - 150, SC_H - 100, 28, (Color){255, 255, 0, alpha});
	alpha += dAlpha;
	if (alpha == 32)
		dAlpha *= -1;
	if (alpha == 248)
		dAlpha *= -1;
}

void	scoreAdd(unsigned short points)
{
	gameState.score += points;
	gameState.oneUpMeter += points;
	if (gameState.oneUpMeter > 10000)
	{
		gameState.numLives++;
		gameState.oneUpMeter -= 10000;
	}
}

void	decayRock(unsigned short rockIdx, bool	shouldScore)
{
	explodeAt(rockPool[rockIdx].hitBox.center);
	switch(rockPool[rockIdx].size)
	{
		case BIG:
			rockPool[rockIdx].size = MEDIUM;
			rockPool[rockIdx].pos.x += 10;
			rockPool[rockIdx].pos.y += 10;
			rockPool[rockIdx].hitBox.radius /= 2;
			rockPool[rockIdx + 2] = rockPool[rockIdx];
			rockPool[rockIdx].vel = Vector2Scale(rockPool[rockIdx].vel, 0.25 * GetRandomValue(4, 8));
			rockPool[rockIdx].vel = Vector2Rotate(rockPool[rockIdx].vel, GetRandomValue(-90, 90) * DEG2RAD);
			rockPool[rockIdx + 2].vel = Vector2Scale(rockPool[rockIdx + 2].vel, 0.25 * GetRandomValue(4, 8));
			rockPool[rockIdx + 2].vel = Vector2Rotate(rockPool[rockIdx + 2].vel, GetRandomValue(-90, 90) * DEG2RAD);
			if (shouldScore)
				scoreAdd(20);
			gameState.rockCount++;
			break;
		case MEDIUM:
			rockPool[rockIdx].size = SMALL;
			rockPool[rockIdx].pos.x += 5;
			rockPool[rockIdx].pos.y += 5;
			rockPool[rockIdx].hitBox.radius /= 2;
			rockPool[rockIdx + 1] = rockPool[rockIdx];
			rockPool[rockIdx].vel = Vector2Scale(rockPool[rockIdx].vel, 0.25 * GetRandomValue(4, 8));
			rockPool[rockIdx].vel = Vector2Rotate(rockPool[rockIdx].vel, GetRandomValue(-90, 90) * DEG2RAD);
			rockPool[rockIdx + 1].vel = Vector2Scale(rockPool[rockIdx + 1].vel, 0.25 * GetRandomValue(4, 8));
			rockPool[rockIdx + 1].vel = Vector2Rotate(rockPool[rockIdx + 1].vel, GetRandomValue(-90, 90) * DEG2RAD);
			if (shouldScore)
				scoreAdd(50);
			gameState.rockCount++;
			break;
		case SMALL:
			rockPool[rockIdx].isLive = false;
			if (shouldScore)
				scoreAdd(100);
			gameState.rockCount--;
			if (gameState.rockCount == 0)
				gameState.cleanTime = GetTime();
			break;
	}
}

void	handlePlayerBulletCollisions()
{
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (bulletPool[i].isLive)
		{
			for (int j = 0; j < ROCK_MAX; j++)
			{
				if (rockPool[j].isLive)
				{
					if (CheckCollisionPointCircle(bulletPool[i].pos, rockPool[j].hitBox.center, rockPool[j].hitBox.radius)
					 || CheckCollisionPointCircle((Vector2){bulletPool[i].pos.x+1, bulletPool[i].pos.y}, rockPool[j].hitBox.center, rockPool[j].hitBox.radius)
					 || CheckCollisionPointCircle((Vector2){bulletPool[i].pos.x+1, bulletPool[i].pos.y+1}, rockPool[j].hitBox.center, rockPool[j].hitBox.radius)
					 || CheckCollisionPointCircle((Vector2){bulletPool[i].pos.x, bulletPool[i].pos.y+1}, rockPool[j].hitBox.center, rockPool[j].hitBox.radius))
					{
						bulletPool[i].isLive = false;
						decayRock(j, true);
					}
				}
			}
			if (saucer.isLive)
			{
				if (CheckCollisionPointCircle(bulletPool[i].pos, saucer.hitBox.center, saucer.hitBox.radius))
				{
					bulletPool[i].isLive = false;
					saucerDie();
					if (saucer.size == BIG)
						scoreAdd(200);
					else
						scoreAdd(1000);
				}
			}
		}
	}
}

void	handlePlayerCollisions()
{
	for (int i = 0; i < ROCK_MAX; i++)
	{
		if (rockPool[i].isLive && player.isLive)
			if (CheckCollisionCircles(player.hitBox.center, player.hitBox.radius, rockPool[i].hitBox.center, rockPool[i].hitBox.radius))
			{
				playerDie();
				decayRock(i, true);
			}
	}
	if (saucer.isLive)
		if (CheckCollisionCircles(player.hitBox.center, player.hitBox.radius, saucer.hitBox.center, saucer.hitBox.radius))
		{
			playerDie();
			saucerDie();
			if (saucer.size == BIG)
				scoreAdd(200);
			else
				scoreAdd(1000);
		}
	if (saucer.bullets[0].isLive)
		if (CheckCollisionPointCircle(saucer.bullets[0].pos, player.hitBox.center, player.hitBox.radius))
		{
			playerDie();
			saucer.bullets[0].isLive = false;
		}
	if (saucer.bullets[1].isLive)
		if (CheckCollisionPointCircle(saucer.bullets[1].pos, player.hitBox.center, player.hitBox.radius))
		{
			playerDie();
			saucer.bullets[1].isLive = false;
		}
}

void	handleSaucerBulletCollisions()
{
	if (saucer.bullets[0].isLive)
	{
		for (int i = 0; i < ROCK_MAX; i++)
		{
			if (rockPool[i].isLive)
			{
				if (CheckCollisionPointCircle(saucer.bullets[0].pos, rockPool[i].hitBox.center, rockPool[i].hitBox.radius))
				{
					saucer.bullets[0].isLive = false;
					decayRock(i, false);
				}
			}
		}
	}
	if (saucer.bullets[1].isLive)
	{
		for (int i = 0; i < ROCK_MAX; i++)
		{
			if (rockPool[i].isLive)
			{
				if (CheckCollisionPointCircle(saucer.bullets[1].pos, rockPool[i].hitBox.center, rockPool[i].hitBox.radius))
				{
					saucer.bullets[1].isLive = false;
					decayRock(i, false);
				}
			}
		}
	}

}

void	handleSaucerCollisions()
{
	for (int i = 0; i < ROCK_MAX; i++)
	{
		if (rockPool[i].isLive)
		{
			if (CheckCollisionCircles(saucer.hitBox.center, saucer.hitBox.radius, rockPool[i].hitBox.center, rockPool[i].hitBox.radius))
			{
				saucerDie();
				decayRock(i, false);
			}
		}
	}
}

void	spawnRocks()
{
	static const Rectangle	spawnAreas[4] = {
		{0, -10, SC_W, 10},
		{SC_W - 10, 0, SC_W + 10, SC_H},
		{0, SC_H - 10, SC_W, SC_H + 10},
		{-10, 0, 10, SC_H}
	};
	static struct {unsigned n: 2;} idx = {};

	if (GetTime() - gameState.cleanTime > 4 && !saucer.isLive)
	{
		for (int i = 0; i < gameState.rocksToSpawn * 4; i += 4)
		{
			rockPool[i] = (t_Rock){
				.hitBox.center.x = GetRandomValue(spawnAreas[idx.n].x, spawnAreas[idx.n].width),
				.hitBox.center.y = GetRandomValue(spawnAreas[idx.n].y, spawnAreas[idx.n].height),
				.hitBox.radius = 20,
				.textureIdx = GetRandomValue(0, 3),
				.size = BIG,
				.vel.x = GetRandomValue(35, 40),
				.isLive = true
			};
			rockPool[i].pos.x = rockPool[i].hitBox.center.x - 20;
			rockPool[i].pos.y = rockPool[i].hitBox.center.y - 20;
			rockPool[i].vel = Vector2Rotate(rockPool[i].vel, GetRandomValue(0, 360) * DEG2RAD);
			idx.n++;
			gameState.rockCount++;
		}
		if (gameState.rocksToSpawn + 2 < ROCK_SPAWN_MAX)
			gameState.rocksToSpawn += 2;
		else
			gameState.rocksToSpawn = ROCK_SPAWN_MAX;
	}
	gameState.levelStartTime = GetTime();
}

void	spawnBgSaucer()
{
	saucer.texture = LoadTexture("resources/Saucer-bg.png");
	saucer.size = BIG;
	saucer.hitBox.radius = 10;
	if (GetRandomValue(0, 1))
	{
		saucer.pos.x = -26;
		saucer.vel.x = saucer.speed;
	}
	else
	{
		saucer.pos.x = SC_W;
		saucer.vel.x = -saucer.speed;
	}
	saucer.pos.y = GetRandomValue(0, SC_H);
	saucer.vel.y = 0;
	saucer.hitBox.center.x = saucer.pos.x + 12.5;
	saucer.hitBox.center.y = saucer.pos.y + 8;
	saucer.spawnTime = GetTime();
	saucer.isLive = true;
}

void	spawnSmSaucer()
{
	saucer.texture = LoadTexture("resources/Saucer-sm.png");
	saucer.size = SMALL;
	saucer.hitBox.radius = 5;
	if (GetRandomValue(0, 1))
	{
		saucer.pos.x = -14;
		saucer.vel.x = saucer.speed;
	}
	else
	{
		saucer.pos.x = SC_W;
		saucer.vel.x = -saucer.speed;
	}
	saucer.pos.y = GetRandomValue(0, SC_H);
	saucer.vel.y = 0;
	saucer.hitBox.center.x = saucer.pos.x + 6.5;
	saucer.hitBox.center.y = saucer.pos.y + 4.5;
	saucer.spawnTime = GetTime();
	saucer.isLive = true;
}

void	spawnSaucer()
{
	double	time = GetTime();

	if ((time - gameState.levelStartTime > 20)
			&& (time - gameState.saucerDeathTime > 10)
			&& (gameState.rockCount <= 8 && gameState.rockCount > 0))
	{
		if (gameState.score > 40000)
			spawnSmSaucer();
		else
		{
			if (gameState.score < 10000)
				spawnBgSaucer();
			else
			{
				if (GetRandomValue(0, 4))
					spawnBgSaucer();
				else
					spawnSmSaucer();
			}
		}
	}
}

void	initSession()
{
	for (int i = 0; i < ROCK_MAX; i++)
		rockPool[i].isLive = 0;
	saucer.isLive = 0;
	gameState.rockCount = 0;
	gameState.rocksToSpawn = 4;
	gameState.numLives = 3;
	gameState.score = 0;
	gameState.oneUpMeter = 0;
	player.angle = 0;
	gameState.deathTime = GetTime() - 2;
	gameState.cleanTime = GetTime() - 2;
	gameState.levelStartTime = GetTime();
	gameState.currentScreen = GAME;
}

void	playerHandleInputs()
{
	if (!gameState.isPaused)
	{
		if (IsKeyDown(KEY_D))
			player.angle += 220 * frameTime;
		if (IsKeyDown(KEY_A))
			player.angle -= 220 * frameTime;
		if (IsKeyDown(KEY_W))
			accelerate();
		else
			decelerate();
		if (IsKeyPressed(KEY_J))
			playerShoot();
		if (IsKeyPressed(KEY_K))
			enterHyperspace();
	}
	if (IsKeyPressed(KEY_SPACE))
		gameState.isPaused = !gameState.isPaused;
}

/************************************ MAIN ************************************/

int	main(void)
{
	InitWindow(SC_W, SC_H, "Asteroids");
	SetTargetFPS(60);
	loadAllTextures();

	while (!WindowShouldClose())
	{
		frameTime = GetFrameTime();
		switch (gameState.currentScreen)
		{
			case TITLE:
				if (IsKeyPressed(KEY_ENTER))
					initSession();
				break;
			case GAME:
				if (player.isLive)
				{
					if (player.inHyperspace)
						exitHyperspace();
					else
					{
						playerHandleInputs();
						handlePlayerCollisions();
					}
				}
				else
				{
					if (gameState.numLives > 0)
						playerRespawn();
					else
						gameState.currentScreen = END;
				}
				if (saucer.isLive)
					handleSaucerCollisions();
				else
					spawnSaucer();
				handlePlayerBulletCollisions();
				handleSaucerBulletCollisions();
				if (gameState.oneUpMeter > 10000)
				{
					gameState.numLives++;
					gameState.oneUpMeter -= 10000;
				}
				if (gameState.rockCount == 0)
					spawnRocks();
				if (!gameState.isPaused)
				{
					updatePlayer();
					updateSaucer();
					updateBullets();
					updateRocks();
					updateExplosions();
				}
				break;
			case END:
			{
				if (GetTime() - gameState.deathTime > 5)
					gameState.currentScreen = TITLE;
				handlePlayerBulletCollisions();
				if (gameState.rockCount == 0)
					spawnRocks();
				updateSaucer();
				updateBullets();
				updateRocks();
				updateExplosions();
				break;
			}
		}

		BeginDrawing();
			ClearBackground(BLACK);
			switch (gameState.currentScreen)
			{
				case TITLE:
					drawTitleScreen();
					break;
				case GAME:
					//DrawLine(SC_W/2, 0, SC_W/2, SC_H, GREEN);
					//DrawLine(0, SC_H/2, SC_W, SC_H/2, GREEN);
					drawPlayer();
					drawBullets();
					drawRocks();
					drawSaucer();
					drawScore();
					drawLives();
					drawExplosions();
					break;
				case END:
					drawBullets();
					drawRocks();
					drawSaucer();
					drawScore();
					drawExplosions();
					DrawText("GAME OVER", SC_W/2 - 98, SC_H/2 - 100, 32, (Color){127, 127, 127, 255});
					break;
			};
		EndDrawing();
	}

	CloseWindow();
	return (0);
}
