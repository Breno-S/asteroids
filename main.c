#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

#define SC_W 640
#define SC_H 480

#define BULLET_MAX 4
#define ROCK_MAX 64

enum e_Size { BIG, MEDIUM, SMALL };

typedef struct	s_Circle
{
	Vector2	center;
	float	radius;
} t_Circle;

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
	Vector2		thrust;
	float		angle;
	float		accel;
	float		maxSpeed;
	bool		isLive;
}	t_PlayerShip;

Texture			rockTextures[12];
t_PlayerShip	player = {
	.currSprite = {0, 0, 21, 10},
	.hitBox = {{SC_W / 2, SC_H / 2}, 5},
	.CoM = {9, 5},
	.pos = {SC_W / 2, SC_H / 2},
	.maxSpeed = 250,
	.accel = 150,
	.isLive = true
};
float			frameTime;
t_Bullet		bulletPool[BULLET_MAX];
t_Rock			rockPool[ROCK_MAX];
unsigned long	gameScore;
unsigned short	numLives = 3;
unsigned short	oneUpMeter;

void	decelerate()
{
	if (Vector2Length(player.thrust) > 0)
	{
		player.thrust = Vector2Subtract(player.thrust, Vector2Scale(player.thrust, 0.2 * frameTime));
	}
}

void	accelerate()
{
	float	speed;
	float	scale;
	
	player.thrust.x += player.accel * cosf(player.angle * DEG2RAD) * frameTime;
	player.thrust.y += player.accel * sinf(player.angle * DEG2RAD) * frameTime;
	speed = Vector2Length((Vector2){player.thrust.x, player.thrust.y});
	if (speed > player.maxSpeed)
	{
		scale = player.maxSpeed / speed;
		player.thrust.x *= scale;
		player.thrust.y *= scale;
	}
}

void	updatePlayer()
{
	player.pos.x += player.thrust.x * frameTime;
	player.pos.y += player.thrust.y * frameTime;
	player.hitBox.center.x += player.thrust.x * frameTime;
	player.hitBox.center.y += player.thrust.y * frameTime;
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
		.x = bulletSpeed * cosf(player.angle *DEG2RAD) + player.thrust.x,
		.y = bulletSpeed * sinf(player.angle *DEG2RAD) + player.thrust.y
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
	if (player.isLive)
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
			//char	stats[32];
			//DrawCircleLinesV(rockPool[i].hitBox.center, rockPool[i].hitBox.radius, RED);
			//sprintf(stats, "%.2f", Vector2Length(rockPool[i].vel)); 
			//DrawText(stats, rockPool[i].pos.x, rockPool[i].pos.y + 30, 10, YELLOW);
		}
	}
}

void	drawScore()
{
	char	scoreStr[15];
	sprintf(scoreStr, "%12.2d", gameScore);
	DrawText(scoreStr, 0, 4, 26, (Color){255, 255, 255, 127});
}

void	drawLives()
{
	for (int i = 0; i < numLives; i++)
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
			rockPool[rockIdx].vel = Vector2Scale(rockPool[rockIdx].vel, 0.25 * GetRandomValue(3, 6));
			rockPool[rockIdx].vel = Vector2Rotate(rockPool[rockIdx].vel, GetRandomValue(-90, 90) * DEG2RAD);
			rockPool[rockIdx + 2].vel = Vector2Scale(rockPool[rockIdx + 2].vel, 0.25 * GetRandomValue(3, 6));
			rockPool[rockIdx + 2].vel = Vector2Rotate(rockPool[rockIdx + 2].vel, GetRandomValue(-90, 90) * DEG2RAD);
			gameScore += 20;
			oneUpMeter += 20;
			break;
		case MEDIUM:
			rockPool[rockIdx].size = SMALL;
			rockPool[rockIdx].pos.x += 5;
			rockPool[rockIdx].pos.y += 5;
			rockPool[rockIdx].hitBox.radius /= 2;
			rockPool[rockIdx + 1] = rockPool[rockIdx];
			rockPool[rockIdx].vel = Vector2Scale(rockPool[rockIdx].vel, 0.25 * GetRandomValue(3, 6));
			rockPool[rockIdx].vel = Vector2Rotate(rockPool[rockIdx].vel, GetRandomValue(-180, 180) * DEG2RAD);
			rockPool[rockIdx + 1].vel = Vector2Scale(rockPool[rockIdx + 1].vel, 0.25 * GetRandomValue(3, 6));
			rockPool[rockIdx + 1].vel = Vector2Rotate(rockPool[rockIdx + 1].vel, GetRandomValue(-180, 180) * DEG2RAD);
			gameScore += 50;
			oneUpMeter += 50;
			break;
		case SMALL:
			rockPool[rockIdx].isLive = false;
			gameScore += 100;
			oneUpMeter += 100;
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
					if (CheckCollisionPointCircle(bulletPool[i].pos, rockPool[j].hitBox.center, rockPool[j].hitBox.radius))
					{
						bulletPool[i].isLive = false;
						decayRock(j);
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
				player.isLive = false;
				numLives--;
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

	for (int i = 0; i < 16; i += 4)
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
		if (GetRandomValue(0, 1))
		{
			rockPool[i].vel.x = GetRandomValue(-40, -30);
			rockPool[i].vel.y = GetRandomValue(-40, -30);
		}
		else
		{
			rockPool[i].vel.x = GetRandomValue(30, 40);
			rockPool[i].vel.y = GetRandomValue(30, 40);
		}
		idx.n++;
	}
}

int	main(void)
{
	InitWindow(SC_W, SC_H, "Asteroids");
	SetTargetFPS(60);
	loadAllTextures();
	spawnRocks();

	while (!WindowShouldClose())
	{
		frameTime = GetFrameTime();

		if (player.isLive)
		{
			if (IsKeyDown(KEY_D))
				player.angle += 210 * frameTime;
			if (IsKeyDown(KEY_A))
				player.angle -= 210 * frameTime;
			if (IsKeyDown(KEY_W))
			{
				player.currSprite.x = 21;
				accelerate();
			}
			else
			{
				player.currSprite.x = 0;
				decelerate();
			}
			if (IsKeyPressed(KEY_SPACE))
				shoot();
		}

		handleBulletRockCollisions();
		handlePlayerRockCollisions();

		if (oneUpMeter > 10000)
		{
			numLives++;
			oneUpMeter -= 10000;
		}
		updatePlayer();
		updateBullets();
		updateRocks();

		BeginDrawing();
			ClearBackground(BLACK);
			//DrawLine(SC_W/2, 0, SC_W/2, SC_H, GREEN);
			//DrawLine(0, SC_H/2, SC_W, SC_H/2, GREEN);
			drawPlayer();
			drawBullets();
			drawRocks();
			drawScore();
			drawLives();
		EndDrawing();
	}
	CloseWindow();
	return (0);
}
