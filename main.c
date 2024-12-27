#include "raylib.h"
#include "raymath.h"
#include "math.h"

#define SC_W 640
#define SC_H 480

#define BULLET_MAX 6
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
	Vector2		CoM;
	Vector2		pos;
	Vector2		thrust;
	float		angle;
	float		accel;
	float		maxSpeed;
}	t_PlayerShip;

Texture			rockTextures[12];
t_PlayerShip	player = {
	.currSprite = {0, 0, 23, 12},
	.CoM = {10, 6},
	.pos = {SC_W / 2, SC_H / 2},
	.maxSpeed = 320,
	.accel = 320
};
float			frameTime;
t_Bullet		bulletPool[BULLET_MAX];
t_Rock			rockPool[ROCK_MAX] = {
	[0] = {.pos = {0,0}, {0, 0}, 0, BIG, true},
	[1] = {.pos = {100,100}, {0, 0}, 1, BIG, true},
	[2] = {.pos = {200,200}, {0, 0}, 2, BIG, true},
	[3] = {.pos = {300,300}, {0, 0}, 3, BIG, true},
};
unsigned short	bulletIdx = 0;

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
	if (player.pos.x > SC_W)
		player.pos.x = 0;
	if (player.pos.y > SC_H)
		player.pos.y = 0;
	if (player.pos.x < 0)
		player.pos.x = SC_W;
	if (player.pos.y < 0)
		player.pos.y = SC_H;
}

void	updateRocks()
{
	for (int i = 0; i < ROCK_MAX; i++)
	{
		rockPool[i].pos.x += rockPool[i].vel.x * frameTime;
		rockPool[i].pos.y += rockPool[i].vel.y * frameTime;
		rockPool[i].pos.x += rockPool[i].vel.x * frameTime;
		rockPool[i].pos.y += rockPool[i].vel.y * frameTime;
		if (rockPool[i].pos.x > SC_W) rockPool[i].pos.x = 0;
		if (rockPool[i].pos.y > SC_H) rockPool[i].pos.y = 0;
		if (rockPool[i].pos.x < 0)    rockPool[i].pos.x = SC_W;
		if (rockPool[i].pos.y < 0)    rockPool[i].pos.y = SC_H;
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
	float	bulletSpeed = 1.25 * player.maxSpeed;
	Vector2	spawn = {
		.x = player.pos.x + 13 * cosf(player.angle * DEG2RAD),
		.y = player.pos.y + 13 * sinf(player.angle * DEG2RAD)
	};
	Vector2	velocity = {
		.x = bulletSpeed * cosf(player.angle *DEG2RAD) + player.thrust.x,
		.y = bulletSpeed * sinf(player.angle *DEG2RAD) + player.thrust.y
	};

	bulletPool[bulletIdx].pos.x = spawn.x;
	bulletPool[bulletIdx].pos.y = spawn.y;
	bulletPool[bulletIdx].vel.x = velocity.x;
	bulletPool[bulletIdx].vel.y = velocity.y;
	bulletPool[bulletIdx].ttl = 1;
	bulletPool[bulletIdx].isLive = true;
	bulletIdx++;
	if (bulletIdx == BULLET_MAX)
		bulletIdx = 0;
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
	DrawTexturePro(
		player.texture,
		player.currSprite,
		(Rectangle){player.pos.x, player.pos.y, player.currSprite.width, player.currSprite.height},
		player.CoM,
		player.angle,
		WHITE
	);
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
			DrawCircleLinesV(rockPool[i].hitBox.center, rockPool[i].hitBox.radius, RED);
		}
	}
}

void	loadHitboxes()
{
	for (int i = 0; i < ROCK_MAX; i++)
	{
		switch (rockPool[i].size)
		{
			case BIG:
				rockPool[i].hitBox.center.x = rockPool[i].pos.x + 20;
				rockPool[i].hitBox.center.y = rockPool[i].pos.y + 20;
				rockPool[i].hitBox.radius = 20;
				break;
			case MEDIUM:
				rockPool[i].hitBox.center.x = rockPool[i].pos.x + 10;
				rockPool[i].hitBox.center.y = rockPool[i].pos.y + 10;
				rockPool[i].hitBox.radius = 10;
				break;
			case SMALL:
				rockPool[i].hitBox.center.x = rockPool[i].pos.x + 5;
				rockPool[i].hitBox.center.y = rockPool[i].pos.y + 5;
				rockPool[i].hitBox.radius = 5;
		};
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
			break;
		case MEDIUM:
			rockPool[rockIdx].size = SMALL;
			rockPool[rockIdx].pos.x += 5;
			rockPool[rockIdx].pos.y += 5;
			break;
		case SMALL:
			rockPool[rockIdx].isLive = false;
			break;
	}
	rockPool[rockIdx].hitBox.radius /= 2;
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
	return false;
}

int	main(void)
{
	InitWindow(SC_W, SC_H, "Asteroids");
	SetTargetFPS(60);
	loadAllTextures();
	loadHitboxes();

	while (!WindowShouldClose())
	{
		frameTime = GetFrameTime();
		player.currSprite.x = 0;

		if (IsKeyDown(KEY_D))
			player.angle += 180 * frameTime;
		if (IsKeyDown(KEY_A))
			player.angle -= 180 * frameTime;
		if (IsKeyDown(KEY_W))
		{
			player.currSprite.x = 23;
			accelerate();
		}
		if (IsKeyPressed(KEY_SPACE))
			shoot();

		handleBulletRockCollisions();

		updatePlayer();
		updateBullets();
		updateRocks();

		BeginDrawing();
			ClearBackground(BLACK);
			drawPlayer();
			drawBullets();
			drawRocks();
		EndDrawing();
	}
	CloseWindow();
	return (0);
}
