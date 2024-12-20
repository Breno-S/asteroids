#include "raylib.h"
#include "raymath.h"
#include "math.h"

#define SC_W 640
#define SC_H 480

#define BULLET_MAX 6
#define ROCK_MAX 64

enum e_Size { BIG, MEDIUM, SMALL };

typedef struct	s_Rock
{
	Vector2	pos;
	Vector2	vel;
	int		textureIdx;
	int		size;
	bool	live;
}	t_Rock;

typedef struct	s_Bullet
{
	Vector2	pos;
	Vector2	vel;
	float	ttl;
	bool	live;
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
	.maxSpeed = 5,
	.accel = 5
};
float			frameTime;
t_Bullet		bulletPool[BULLET_MAX];
t_Rock			rockPool[ROCK_MAX];
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
	player.pos.x += player.thrust.x;
	player.pos.y += player.thrust.y;
	if (player.pos.x > SC_W)
		player.pos.x = 0;
	if (player.pos.y > SC_H)
		player.pos.y = 0;
	if (player.pos.x < 0)
		player.pos.x = SC_W;
	if (player.pos.y < 0)
		player.pos.y = SC_H;
}

void	updateBullets()
{
	for (int i = 0; i < BULLET_MAX; i++)
	{
		bulletPool[i].pos.x += bulletPool[i].vel.x;
		bulletPool[i].pos.y += bulletPool[i].vel.y;
		if (bulletPool[i].pos.x > SC_W) bulletPool[i].pos.x = 0;
		if (bulletPool[i].pos.y > SC_H) bulletPool[i].pos.y = 0;
		if (bulletPool[i].pos.x < 0)    bulletPool[i].pos.x = SC_W;
		if (bulletPool[i].pos.y < 0)    bulletPool[i].pos.y = SC_H;
		if (bulletPool[i].live)
		{
			bulletPool[i].ttl -= frameTime;
			if (bulletPool[i].ttl <= 0.0F)
				bulletPool[i].live = false;
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
	bulletPool[bulletIdx].live = true;
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

int	main(void)
{
	InitWindow(SC_W, SC_H, "Asteroids");
	SetTargetFPS(60);
	loadAllTextures();

	while (!WindowShouldClose())
	{
		frameTime = GetFrameTime();
		player.currSprite.x = 0;

		if (IsKeyDown(KEY_D))
			player.angle += 3;
		if (IsKeyDown(KEY_A))
			player.angle -= 3;
		if (IsKeyDown(KEY_W))
		{
			player.currSprite.x = 23;
			accelerate();
		}
		if (IsKeyPressed(KEY_SPACE))
			shoot();
		
		updatePlayer();
		updateBullets();

		BeginDrawing();
			ClearBackground(BLACK);
			//DrawLine(SC_W / 2, 0, SC_W / 2, SC_H, WHITE);
			//DrawLine(0, SC_H / 2, SC_W, SC_H / 2, WHITE);
			DrawTexturePro(
				player.texture,
				player.currSprite,
				(Rectangle){player.pos.x, player.pos.y, player.currSprite.width, player.currSprite.height},
				player.CoM,
				player.angle,
				WHITE
			);
			for (int i = 0; i < BULLET_MAX; i++)
				if (bulletPool[i].live)
				{
					DrawPixel(bulletPool[i].pos.x, bulletPool[i].pos.y, WHITE);
				}
			for (int i = 0; i < ROCK_MAX; i++)
			{
				if (rockPool[i].live)
				{
					DrawTexture(
						rockTextures[rockPool[i].textureIdx + 4 * rockPool[i].size],
						rockPool[i].pos.x,
						rockPool[i].pos.y,
						WHITE
					);
				}
			}
		EndDrawing();
	}
	CloseWindow();
	return (0);
}
