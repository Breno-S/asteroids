#include "raylib.h"
#include "raymath.h"
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
	float				deathTime;
	float				cleanTime;
	float				hyperspaceTime;
	unsigned short		numLives;
	unsigned short		oneUpMeter;
	unsigned short		rockCount;
	unsigned short		rocksToSpawn;
	enum e_GameScreen	currentScreen;
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
	int			size;
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
	.CoM = {9, 5},
	.pos = {SC_W / 2, SC_H / 2},
	.maxSpeed = 220,
	.accel = 150,
};
t_Saucer	mdSaucer = {
	.hitBox = {{112, 108}, 10},
	.speed = 70
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
	float					bulletSpeed = player.maxSpeed * 1;

	for (int i = 0; i < BULLET_MAX; i++)
		if (bulletPool[i].isLive == false)
			isFull = false;
	if (!isFull)
	{
		bulletPool[bulletIdx].pos = (Vector2){
			.x = player.pos.x + 13 * cosf(player.angle * DEG2RAD),
			.y = player.pos.y + 13 * sinf(player.angle * DEG2RAD)
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

void	mdSaucerShoot()
{
	static bool	bulletIdx = 0;
	float		bulletSpeed = player.maxSpeed * 1.25;
	float		angle = GetRandomValue(0, 360) * DEG2RAD;

	if (!mdSaucer.bullets[bulletIdx].isLive)
	{
		mdSaucer.bullets[bulletIdx].pos = (Vector2){
			.x = mdSaucer.hitBox.center.x + 5 * cosf(angle),
			.y = mdSaucer.hitBox.center.y + 5 * sinf(angle)
		};
		mdSaucer.bullets[bulletIdx].vel = (Vector2){
			.x = bulletSpeed * cosf(angle),
			.y = bulletSpeed * sinf(angle)
		};
		mdSaucer.bullets[bulletIdx].ttl = 1;
		mdSaucer.bullets[bulletIdx].isLive = true;
		bulletIdx = !bulletIdx;
		mdSaucer.shootTime = GetTime();
	}
}

void	enterHyperspace()
{
	gameState.hyperspaceTime = GetTime();
	player.inHyperspace = true;
}

void	exitHyperspace()
{
	if (GetTime() - gameState.hyperspaceTime > 2)
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
		player.isLive = false;
		player.vel.x = 0;
		player.vel.y = 0;
		player.pos.x = SC_W / 2;
		player.pos.y = SC_H / 2;
		player.hitBox.center.x = SC_W / 2;
		player.hitBox.center.y = SC_H / 2;
		gameState.numLives--;
		gameState.deathTime = GetTime();
}

void	mdSaucerDie()
{
	mdSaucer.isLive = false;
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
		player.isLive = true;
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

void	updateMdSaucer()
{
	short angles[3] = {-35, 0, 35};
	static struct {unsigned n: 2;} idx = {};

	if (mdSaucer.isLive)
	{
		mdSaucer.pos = Vector2Add(mdSaucer.pos, Vector2Scale(mdSaucer.vel, frameTime));
		mdSaucer.hitBox.center = Vector2Add(mdSaucer.hitBox.center, Vector2Scale(mdSaucer.vel, frameTime));
		if (GetTime() - mdSaucer.spawnTime > 2)
		{
			if (mdSaucer.vel.x > 0)
			{
				if (idx.n == 0)
					mdSaucer.vel = Vector2Rotate((Vector2){mdSaucer.speed, 0}, angles[GetRandomValue(1, 2)] * DEG2RAD);
				else if (idx.n == 1)
					mdSaucer.vel = Vector2Rotate((Vector2){mdSaucer.speed, 0}, angles[GetRandomValue(0, 2)] * DEG2RAD);
				else
					mdSaucer.vel = Vector2Rotate((Vector2){mdSaucer.speed, 0}, angles[GetRandomValue(0, 1)] * DEG2RAD);
			}
			else
			{
				if (idx.n == 0)
					mdSaucer.vel = Vector2Rotate((Vector2){mdSaucer.speed, 0}, (angles[GetRandomValue(1, 2)] + 180) * DEG2RAD);
				else if (idx.n == 1)
					mdSaucer.vel = Vector2Rotate((Vector2){mdSaucer.speed, 0}, (angles[GetRandomValue(0, 2)] + 180) * DEG2RAD);
				else
					mdSaucer.vel = Vector2Rotate((Vector2){mdSaucer.speed, 0}, (angles[GetRandomValue(0, 1)] + 180) * DEG2RAD);
			}
			idx.n++;
			mdSaucer.spawnTime = GetTime();
		}
		if (mdSaucer.hitBox.center.y > SC_H)
		{
			mdSaucer.hitBox.center.y = 0;
			mdSaucer.pos.y = - 8;
		}
		if (mdSaucer.hitBox.center.y < 0)
		{
			mdSaucer.hitBox.center.y = SC_H;
			mdSaucer.pos.y = SC_H - 8;
		}
		if (mdSaucer.pos.x > SC_W + 1 || mdSaucer.pos.x < -25)
			mdSaucer.isLive = false;
		if (GetTime() - mdSaucer.shootTime > 0.66)
			mdSaucerShoot();
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
	if (mdSaucer.bullets[0].isLive)
	{
		mdSaucer.bullets[0].pos = Vector2Add(mdSaucer.bullets[0].pos, Vector2Scale(mdSaucer.bullets[0].vel, frameTime));
		mdSaucer.bullets[0].ttl -= frameTime;
		if (mdSaucer.bullets[0].ttl <= 0.0F)
			mdSaucer.bullets[0].isLive = false;
	}
	if (mdSaucer.bullets[1].isLive)
	{
		mdSaucer.bullets[1].pos = Vector2Add(mdSaucer.bullets[1].pos, Vector2Scale(mdSaucer.bullets[1].vel, frameTime));
		mdSaucer.bullets[1].ttl -= frameTime;
		if (mdSaucer.bullets[1].ttl <= 0.0F)
			mdSaucer.bullets[1].isLive = false;
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
	mdSaucer.texture = LoadTexture("resources/Saucer-md.png");
	//smallSaucerTexture = LoadTexture("resources/Saucer-sm.png");
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
		//DrawCircleLinesV(player.hitBox.center, player.hitBox.radius, RED);
	}
}

void	drawMdSaucer()
{
	if (mdSaucer.isLive)
	{
		DrawTexture(
			mdSaucer.texture,
			mdSaucer.pos.x,
			mdSaucer.pos.y,
			WHITE
		);
		DrawCircleLinesV(mdSaucer.hitBox.center, mdSaucer.hitBox.radius, RED);
	}
}

void	drawBullets()
{
	for (int i = 0; i < BULLET_MAX; i++)
		if (bulletPool[i].isLive)
			DrawPixel(bulletPool[i].pos.x, bulletPool[i].pos.y, WHITE);
	if (mdSaucer.bullets[0].isLive)
		DrawPixel(mdSaucer.bullets[0].pos.x, mdSaucer.bullets[0].pos.y, WHITE);
	if (mdSaucer.bullets[1].isLive)
		DrawPixel(mdSaucer.bullets[1].pos.x, mdSaucer.bullets[1].pos.y, WHITE);
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
			//DrawCircleLinesV(rockPool[i].hitBox.center, rockPool[i].hitBox.radius, RED);
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
			return;
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
					if (CheckCollisionPointCircle(bulletPool[i].pos, rockPool[j].hitBox.center, rockPool[j].hitBox.radius))
					{
						bulletPool[i].isLive = false;
						decayRock(j, true);
					}
				}
			}
			if (mdSaucer.isLive)
			{
				if (CheckCollisionPointCircle(bulletPool[i].pos, mdSaucer.hitBox.center, mdSaucer.hitBox.radius))
				{
					bulletPool[i].isLive = false;
					mdSaucerDie();
					scoreAdd(200);
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
	if (mdSaucer.isLive)
		if (CheckCollisionCircles(player.hitBox.center, player.hitBox.radius, mdSaucer.hitBox.center, mdSaucer.hitBox.radius))
		{
			playerDie();
			mdSaucerDie();
			scoreAdd(200);
		}
	if (mdSaucer.bullets[0].isLive)
		if (CheckCollisionPointCircle(mdSaucer.bullets[0].pos, player.hitBox.center, player.hitBox.radius))
		{
			playerDie();
			mdSaucer.bullets[0].isLive = false;
		}
	if (mdSaucer.bullets[1].isLive)
		if (CheckCollisionPointCircle(mdSaucer.bullets[1].pos, player.hitBox.center, player.hitBox.radius))
		{
			playerDie();
			mdSaucer.bullets[1].isLive = false;
		}
}

void	handleMdSaucerBulletCollisions()
{
	if (mdSaucer.bullets[0].isLive)
	{
		for (int i = 0; i < ROCK_MAX; i++)
		{
			if (rockPool[i].isLive)
			{
				if (CheckCollisionPointCircle(mdSaucer.bullets[0].pos, rockPool[i].hitBox.center, rockPool[i].hitBox.radius))
				{
					mdSaucer.bullets[0].isLive = false;
					decayRock(i, false);
				}
			}
		}
	}
	if (mdSaucer.bullets[1].isLive)
	{
		for (int i = 0; i < ROCK_MAX; i++)
		{
			if (rockPool[i].isLive)
			{
				if (CheckCollisionPointCircle(mdSaucer.bullets[1].pos, rockPool[i].hitBox.center, rockPool[i].hitBox.radius))
				{
					mdSaucer.bullets[1].isLive = false;
					decayRock(i, false);
				}
			}
		}
	}

}

void	handleMdSaucerCollisions()
{
	for (int i = 0; i < ROCK_MAX; i++)
	{
		if (rockPool[i].isLive)
		{
			if (CheckCollisionCircles(mdSaucer.hitBox.center, mdSaucer.hitBox.radius, rockPool[i].hitBox.center, rockPool[i].hitBox.radius))
			{
				mdSaucerDie();
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

	if (GetTime() - gameState.cleanTime > 4 && !mdSaucer.isLive)
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
}

void	spawnMdSaucer()
{
	if (GetRandomValue(0, 1))
	{
		mdSaucer.pos.x = -24;
		mdSaucer.vel.x = mdSaucer.speed;
	}
	else
	{
		mdSaucer.pos.x = SC_W;
		mdSaucer.vel.x = -mdSaucer.speed;
	}
	mdSaucer.pos.y = GetRandomValue(0, SC_H);
	mdSaucer.vel.y = 0;
	mdSaucer.hitBox.center.x = mdSaucer.pos.x + 12;
	mdSaucer.hitBox.center.y = mdSaucer.pos.y + 8;
	mdSaucer.spawnTime = GetTime();
	mdSaucer.isLive = true;
}

void	initSession()
{
	for (int i = 0; i < ROCK_MAX; i++)
		rockPool[i].isLive = 0;
	gameState.rockCount = 0;
	gameState.rocksToSpawn = 4;
	gameState.numLives = 3;
	gameState.score = 0;
	gameState.oneUpMeter = 0;
	player.angle = 0;
	gameState.deathTime = GetTime() - 2;
	gameState.cleanTime = GetTime() - 2;
	gameState.currentScreen = GAME;
}

void	playerHandleInputs()
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
				if (mdSaucer.isLive)
					handleMdSaucerCollisions();

				handlePlayerBulletCollisions();
				handleMdSaucerBulletCollisions();
				if (gameState.oneUpMeter > 10000)
				{
					gameState.numLives++;
					gameState.oneUpMeter -= 10000;
				}
				if (gameState.rockCount == 0)
					spawnRocks();

				updatePlayer();
				updateMdSaucer();
				updateBullets();
				updateRocks();
				break;
			case END:
			{
				if (GetTime() - gameState.deathTime > 5)
					gameState.currentScreen = TITLE;
				handlePlayerBulletCollisions();
				if (gameState.rockCount == 0)
					spawnRocks();

				updateBullets();
				updateRocks();
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
					drawMdSaucer();
					drawScore();
					drawLives();
					break;
				case END:
					drawBullets();
					drawRocks();
					drawScore();
					DrawText("GAME OVER", SC_W/2 - 98, SC_H/2 - 100, 32, (Color){127, 127, 127, 255});
					break;
			};
		EndDrawing();
	}

	CloseWindow();
	return (0);
}
