#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

#define SC_W 640
#define SC_H 480

#define BULLET_MAX 4
#define ROCK_MAX 64

enum e_Size { BIG, MEDIUM, SMALL };
enum e_GameScreen { TITLE, GAME, END };

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
	t_Bullet	bullet;
	Texture		texture;
	t_Circle	hitBox;
	Vector2		pos;
	Vector2		vel;
	double		spawnTime;
	bool		isLive;
}	t_Saucer;

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
	.maxSpeed = 200,
	.accel = 150,
};
t_Saucer	mediumSaucer = {
	.pos = {100, 100},
	.hitBox = {{112, 108}, 10},
};
float			frameTime;
t_Bullet		bulletPool[BULLET_MAX];
t_Rock			rockPool[ROCK_MAX];

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

void	shoot()
{
	static unsigned	short	bulletIdx = 0;
	bool					isFull = true;
	float					bulletSpeed = player.maxSpeed;
	Vector2					spawn = {
		.x = player.pos.x + 13 * cosf(player.angle * DEG2RAD),
		.y = player.pos.y + 13 * sinf(player.angle * DEG2RAD)
	};
	Vector2					velocity = {
		.x = bulletSpeed * cosf(player.angle *DEG2RAD) + player.vel.x,
		.y = bulletSpeed * sinf(player.angle *DEG2RAD) + player.vel.y
	};

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (bulletPool[i].isLive == false)
			isFull = false;
	}
	if (!isFull)
	{
		bulletPool[bulletIdx].pos.x = spawn.x;
		bulletPool[bulletIdx].pos.y = spawn.y;
		bulletPool[bulletIdx].vel.x = velocity.x;
		bulletPool[bulletIdx].vel.y = velocity.y;
		bulletPool[bulletIdx].ttl = 1.2;
		bulletPool[bulletIdx].isLive = true;
		bulletIdx++;
		if (bulletIdx == BULLET_MAX)
			bulletIdx = 0;
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

void	updateMediumSaucer()
{
	short angles[3] = {-45, 0, 45};
	mediumSaucer.pos.x += mediumSaucer.vel.x * frameTime;
	mediumSaucer.pos.y += mediumSaucer.vel.y * frameTime;
	mediumSaucer.hitBox.center.x += mediumSaucer.vel.x * frameTime;
	mediumSaucer.hitBox.center.y += mediumSaucer.vel.y * frameTime;

	if (GetTime() - mediumSaucer.spawnTime > 2)
	{
		if (mediumSaucer.vel.x > 0)
			mediumSaucer.vel = Vector2Rotate((Vector2){60, 0}, angles[GetRandomValue(0, 2)] * DEG2RAD);
		else
			mediumSaucer.vel = Vector2Rotate((Vector2){60, 0}, (angles[GetRandomValue(0, 2)] + 180) * DEG2RAD);
		mediumSaucer.spawnTime = GetTime();
	}

	if (mediumSaucer.hitBox.center.y > SC_H)
	{
		mediumSaucer.hitBox.center.y = 0;
		mediumSaucer.pos.y = - 8;
	}
	if (mediumSaucer.hitBox.center.y < 0)
	{
		mediumSaucer.hitBox.center.y = SC_H;
		mediumSaucer.pos.y = SC_H - 8;
	}
	if (mediumSaucer.pos.x > SC_W + 1 || mediumSaucer.pos.x < -25)
		mediumSaucer.isLive = false;
}

void	updateRocks()
{
	for (int i = 0; i < ROCK_MAX; i++)
	{
		rockPool[i].pos.x += rockPool[i].vel.x * frameTime;
		rockPool[i].pos.y += rockPool[i].vel.y * frameTime;
		rockPool[i].hitBox.center.x += rockPool[i].vel.x * frameTime;
		rockPool[i].hitBox.center.y += rockPool[i].vel.y * frameTime;
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
		bulletPool[i].pos.x += bulletPool[i].vel.x * frameTime;
		bulletPool[i].pos.y += bulletPool[i].vel.y * frameTime;
		if (bulletPool[i].pos.x > SC_W) bulletPool[i].pos.x = 0;
		if (bulletPool[i].pos.y > SC_H) bulletPool[i].pos.y = 0;
		if (bulletPool[i].pos.x < 0)    bulletPool[i].pos.x = SC_W;
		if (bulletPool[i].pos.y < 0)    bulletPool[i].pos.y = SC_H;
		if (bulletPool[i].isLive)
		{
			bulletPool[i].ttl -= frameTime;
			if (bulletPool[i].ttl <= 0.0F)
				bulletPool[i].isLive = false;
		}
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
	mediumSaucer.texture = LoadTexture("resources/Saucer-md.png");
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

void	drawMediumSaucer()
{
	if (mediumSaucer.isLive)
	{
		DrawTexture(
			mediumSaucer.texture,
			mediumSaucer.pos.x,
			mediumSaucer.pos.y,
			WHITE
		);
		DrawCircleLinesV(mediumSaucer.hitBox.center, mediumSaucer.hitBox.radius, RED);
	}
}

void	drawBullets()
{
	for (int i = 0; i < BULLET_MAX; i++)
		if (bulletPool[i].isLive)
			DrawPixel(bulletPool[i].pos.x, bulletPool[i].pos.y, WHITE);
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

void	decayRock(unsigned short rockIdx)
{
	switch(rockPool[rockIdx].size)
	{
		case BIG:
			rockPool[rockIdx].size = MEDIUM;
			rockPool[rockIdx].pos.x += 10;
			rockPool[rockIdx].pos.y += 10;
			rockPool[rockIdx].hitBox.radius /= 2;
			rockPool[rockIdx + 2] = rockPool[rockIdx];
			rockPool[rockIdx].vel = Vector2Scale(rockPool[rockIdx].vel, 0.25 * GetRandomValue(4, 7));
			rockPool[rockIdx].vel = Vector2Rotate(rockPool[rockIdx].vel, GetRandomValue(-90, 90) * DEG2RAD);
			rockPool[rockIdx + 2].vel = Vector2Scale(rockPool[rockIdx + 2].vel, 0.25 * GetRandomValue(4, 7));
			rockPool[rockIdx + 2].vel = Vector2Rotate(rockPool[rockIdx + 2].vel, GetRandomValue(-90, 90) * DEG2RAD);
			gameState.score += 20;
			gameState.oneUpMeter += 20;
			gameState.rockCount++;
			break;
		case MEDIUM:
			rockPool[rockIdx].size = SMALL;
			rockPool[rockIdx].pos.x += 5;
			rockPool[rockIdx].pos.y += 5;
			rockPool[rockIdx].hitBox.radius /= 2;
			rockPool[rockIdx + 1] = rockPool[rockIdx];
			rockPool[rockIdx].vel = Vector2Scale(rockPool[rockIdx].vel, 0.25 * GetRandomValue(4, 7));
			rockPool[rockIdx].vel = Vector2Rotate(rockPool[rockIdx].vel, GetRandomValue(-180, 180) * DEG2RAD);
			rockPool[rockIdx + 1].vel = Vector2Scale(rockPool[rockIdx + 1].vel, 0.25 * GetRandomValue(4, 7));
			rockPool[rockIdx + 1].vel = Vector2Rotate(rockPool[rockIdx + 1].vel, GetRandomValue(-180, 180) * DEG2RAD);
			gameState.score += 50;
			gameState.oneUpMeter += 50;
			gameState.rockCount++;
			break;
		case SMALL:
			rockPool[rockIdx].isLive = false;
			gameState.score += 100;
			gameState.oneUpMeter += 100;
			gameState.rockCount--;
			if (gameState.rockCount == 0)
				gameState.cleanTime = GetTime();
			return;
	}
}

void	handleBulletRockCollisions()
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
						decayRock(j);
						spawnMediumSaucer();
					}
				}
			}
		}
	}
}

void	handlePlayerRockCollisions()
{
	for (int i = 0; i < ROCK_MAX; i++)
	{
		if (rockPool[i].isLive && player.isLive)
			if (CheckCollisionCircles(player.hitBox.center, player.hitBox.radius, rockPool[i].hitBox.center, rockPool[i].hitBox.radius))
			{
				playerDie();
				decayRock(i);
			}
	}
}

void	spawnRocks()
{
	static const Rectangle	spawnAreas[4] = {
		{0, 0, SC_W, 80},
		{SC_W - 80, 0, SC_W, SC_H},
		{0, SC_H - 80, SC_W, SC_H},
		{0, 0, 80, SC_H}
	};
	static struct {unsigned n: 2;} idx = {};

	if (GetTime() - gameState.cleanTime > 5)
	{
		for (int i = 0; i < gameState.rocksToSpawn * 4; i += 4)
		{
			rockPool[i] = (t_Rock){
				.pos = {
					GetRandomValue(spawnAreas[idx.n].x, spawnAreas[idx.n].width), 
					GetRandomValue(spawnAreas[idx.n].y, spawnAreas[idx.n].height)
				},
				.textureIdx = GetRandomValue(0, 3),
				.size = BIG,
				.isLive = true
			};
			rockPool[i].hitBox.center.x = rockPool[i].pos.x + 20;
			rockPool[i].hitBox.center.y = rockPool[i].pos.y + 20;
			rockPool[i].hitBox.radius = 20;
			rockPool[i].vel.x = GetRandomValue(30, 40);
			rockPool[i].vel = Vector2Rotate(rockPool[i].vel, GetRandomValue(0, 360) * DEG2RAD);
			idx.n++;
			gameState.rockCount++;
		}
		if (gameState.rocksToSpawn < ROCK_MAX / 4)
			gameState.rocksToSpawn += 2;
	}
}

void	spawnMediumSaucer()
{
	if (GetRandomValue(0, 1))
	{
		mediumSaucer.pos.x = -24;
		mediumSaucer.vel.x = 60;
	}
	else
	{
		mediumSaucer.pos.x = SC_W;
		mediumSaucer.vel.x = -60;
	}
	mediumSaucer.pos.y = GetRandomValue(0, SC_H);
	mediumSaucer.vel.y = 0;
	mediumSaucer.hitBox.center.x = mediumSaucer.pos.x + 12;
	mediumSaucer.hitBox.center.y = mediumSaucer.pos.y + 8;
	mediumSaucer.spawnTime = GetTime();
	mediumSaucer.isLive = true;
}

void	initSession()
{
	for (int i = 0; i < ROCK_MAX; i++)
	{
		rockPool[i].isLive = 0;
	}
	gameState.rockCount = 0;
	gameState.rocksToSpawn = 4;
	gameState.numLives = 3;
	gameState.score = 0;
	gameState.oneUpMeter = 0;
	player.angle = 0;
	gameState.deathTime = GetTime() - 2;
	gameState.cleanTime = GetTime() - 3;
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
		shoot();
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
						handlePlayerRockCollisions();
					}
				}
				else
				{
					if (gameState.numLives > 0)
						playerRespawn();
					else
						gameState.currentScreen = END;
				}

				handleBulletRockCollisions();
				if (gameState.oneUpMeter > 10000)
				{
					gameState.numLives++;
					gameState.oneUpMeter -= 10000;
				}
				if (gameState.rockCount == 0)
					spawnRocks();

				updatePlayer();
				updateMediumSaucer();
				updateBullets();
				updateRocks();
				break;
			case END:
			{
				if (GetTime() - gameState.deathTime > 5)
					gameState.currentScreen = TITLE;
				handleBulletRockCollisions();
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
					drawMediumSaucer();
					drawScore();
					drawLives();
					break;
				case END:
					drawBullets();
					drawRocks();
					drawScore();
					DrawText("GAME OVER", SC_W/2 - 150, SC_H/2 - 30, 50, (Color){127, 127, 127, 255});
					break;
			};
		EndDrawing();
	}

	CloseWindow();
	return (0);
}
