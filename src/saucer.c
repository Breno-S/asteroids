#include <math.h>

#include "explosions.h"
#include "gameState.h"
#include "main.h"
#include "playerShip.h"
#include "raylib.h"
#include "raymath.h"
#include "resources.h"
#include "rocks.h"
#include "saucer.h"

t_Saucer	g_saucer = {
	.speed = 70,
	.inaccuracy = 10.0F,
	.mourningTimer = 10,
};

static Vector2	getBestTarget(void)
{
	Vector2	best = g_player.pos;

	if (fabsf(g_saucer.pos.x - g_player.pos.x) > SC_W / 2)
		best.x += (g_player.pos.x < SC_W / 2) ? SC_W : -SC_W;
	if (fabsf(g_saucer.pos.y - g_player.pos.y) > SC_H / 2)
		best.y += (g_player.pos.y < SC_W / 2) ? SC_H : -SC_H;
	return (best);
}

void	saucerShoot(void)
{
	static bool	bulletIdx = 0;
	float		bulletSpeed = g_player.maxSpeed;
	float		angle;

	if (!g_saucer.bullets[bulletIdx].isLive)
	{
		switch (g_saucer.size)
		{
			case BIG:
				angle = GetRandomValue(0, 360) * DEG2RAD;
				break;
			case SMALL:
				angle = Vector2LineAngle(g_saucer.hitBox.center, getBestTarget())
					+ g_saucer.inaccuracy * DEG2RAD;
				break;
			default:
				break;
		}
		g_saucer.bullets[bulletIdx].pos = (Vector2){
			.x = g_saucer.hitBox.center.x + 5 * cosf(angle),
			.y = g_saucer.hitBox.center.y + 5 * sinf(angle)
		};
		g_saucer.bullets[bulletIdx].vel = (Vector2){
			.x = bulletSpeed * cosf(angle),
			.y = bulletSpeed * -sinf(angle)
		};
		g_saucer.bullets[bulletIdx].ttl = 1.3;
		g_saucer.bullets[bulletIdx].isLive = true;
		bulletIdx = !bulletIdx;
		g_saucer.shootTime = g_time;
		PlaySound(g_sounds.saucerFireSFX);
	}
	if (GetRandomValue(0, 1))
		g_saucer.inaccuracy *= -1;
}

void	saucerDie(void)
{
	explodeAt(g_saucer.hitBox.center);
	g_saucer.isLive = false;
	if (g_saucer.size == SMALL && g_saucer.inaccuracy != 0)
		g_saucer.inaccuracy += (g_saucer.inaccuracy < 0) ? 0.5F : -0.5F;
	g_saucer.deathTime = g_time;
	g_saucer.shouldRespawn = false;
	if (g_saucer.mourningTimer > 0)
		g_saucer.mourningTimer -= 1;
	switch (g_saucer.size)
	{
		case BIG:
			PlaySound(g_sounds.bangBgSFX);
			break;
		case SMALL:
			PlaySound(g_sounds.bangMdSFX);
			break;
		default:
			break;
	}
}

static void	spawnBgSaucer(void)
{
	g_saucer.size = BIG;
	g_saucer.hitBox.radius = 10;
	if (GetRandomValue(0, 1))
	{
		g_saucer.pos.x = -26;
		g_saucer.vel.x = g_saucer.speed;
	}
	else
	{
		g_saucer.pos.x = SC_W;
		g_saucer.vel.x = -g_saucer.speed;
	}
	g_saucer.pos.y = GetRandomValue(
		g_saucer.hitBox.radius * 2,
		SC_H - g_saucer.hitBox.radius * 2
	);
	g_saucer.vel.y = 0;
	g_saucer.hitBox.center.x = g_saucer.pos.x + 12.5;
	g_saucer.hitBox.center.y = g_saucer.pos.y + 8;
	g_saucer.spawnTime = g_time;
	g_saucer.isLive = true;
}

static void	spawnSmSaucer(void)
{
	g_saucer.size = SMALL;
	g_saucer.hitBox.radius = 5;
	if (GetRandomValue(0, 1))
	{
		g_saucer.pos.x = -14;
		g_saucer.vel.x = g_saucer.speed;
	}
	else
	{
		g_saucer.pos.x = SC_W;
		g_saucer.vel.x = -g_saucer.speed;
	}
	g_saucer.pos.y = GetRandomValue(
		g_saucer.hitBox.radius * 2,
		SC_H - g_saucer.hitBox.radius * 2
	);
	g_saucer.vel.y = 0;
	g_saucer.hitBox.center.x = g_saucer.pos.x + 6.5;
	g_saucer.hitBox.center.y = g_saucer.pos.y + 4.5;
	g_saucer.spawnTime = g_time;
	g_saucer.isLive = true;
}

void	requestSaucer(void)
{
	bool	isValid;

	if ((g_time - g_gameState.levelStartTime > 8)
		&& (!g_saucer.isLive)
		&& (g_time - g_saucer.deathTime > g_saucer.mourningTimer)
		&& ((g_gameState.rockCount <= 8 && g_gameState.rockCount > 0)
			|| g_gameState.currentScreen == POSTGAME))
	{
		isValid = true;
	}
	else
		isValid = false;
	if (isValid && !g_saucer.shouldRespawn)
	{
		g_saucer.shouldRespawn = true;
		g_saucer.requestTime = g_time;
	}
	if (!isValid)
		g_saucer.shouldRespawn = false;
	if (g_saucer.shouldRespawn
		&& (g_time - g_saucer.requestTime > 4)
		&& (g_time - g_player.spawnTime > 8))
	{
		if (g_gameState.score > 40000)
			spawnSmSaucer();
		else
		{
			if (g_time - g_gameState.levelStartTime > 60)
			{
				if (GetRandomValue(0, 9))
					spawnSmSaucer();
				else
					spawnBgSaucer();
			}
			else
			{
				if (g_gameState.score < 10000)
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
		g_saucer.spawnTime = g_time;
		g_saucer.shouldRespawn = false;
	}
}

void	handleSaucerCollisions(void)
{
	for (int i = 0; i < ROCK_MAX; i++)
	{
		if (g_rocks[i].isLive)
		{
			if (CheckCollisionCircles(
					g_saucer.hitBox.center,
					g_saucer.hitBox.radius,
					g_rocks[i].hitBox.center,
					g_rocks[i].hitBox.radius))
			{
				saucerDie();
				decayRock(i, false);
			}
		}
	}
}

void	handleSaucerBulletCollisions(void)
{
	if (g_saucer.bullets[0].isLive)
	{
		for (int i = 0; i < ROCK_MAX; i++)
		{
			if (g_rocks[i].isLive)
			{
				if (CheckCollisionPointCircle(
						g_saucer.bullets[0].pos,
						g_rocks[i].hitBox.center,
						g_rocks[i].hitBox.radius))
				{
					g_saucer.bullets[0].isLive = false;
					decayRock(i, false);
				}
			}
		}
	}
	if (g_saucer.bullets[1].isLive)
	{
		for (int i = 0; i < ROCK_MAX; i++)
		{
			if (g_rocks[i].isLive)
			{
				if (CheckCollisionPointCircle(
						g_saucer.bullets[1].pos,
						g_rocks[i].hitBox.center,
						g_rocks[i].hitBox.radius))
				{
					g_saucer.bullets[1].isLive = false;
					decayRock(i, false);
				}
			}
		}
	}
}

void	updateSaucer(void)
{
	static const short				yComponent[3] = {-70, 0, 70};
	static struct {unsigned n: 2;}	idx = {0};

	if (g_saucer.isLive)
	{
		g_saucer.pos = Vector2Add(
			g_saucer.pos,
			Vector2Scale(g_saucer.vel, g_frameTime)
		);
		g_saucer.hitBox.center = Vector2Add(
			g_saucer.hitBox.center,
			Vector2Scale(g_saucer.vel, g_frameTime)
		);
		if (g_time - g_saucer.spawnTime > 2)
		{
			if (idx.n == 0)
				g_saucer.vel.y = yComponent[GetRandomValue(1, 2)];
			else if (idx.n == 1)
				g_saucer.vel.y = yComponent[GetRandomValue(0, 2)];
			else
				g_saucer.vel.y = yComponent[GetRandomValue(0, 1)];
			idx.n++;
			g_saucer.spawnTime = g_time;
		}
		if (g_saucer.hitBox.center.y > SC_H)
		{
			g_saucer.hitBox.center.y = 0;
			g_saucer.pos.y = (g_saucer.size == BIG) ? -8 : -4;
		}
		if (g_saucer.hitBox.center.y < 0)
		{
			g_saucer.hitBox.center.y = SC_H;
			g_saucer.pos.y = (g_saucer.size == BIG) ? SC_H - 8 : SC_H - 4;
		}
		if (g_saucer.pos.x > SC_W + 1 || g_saucer.pos.x < -25)
		{
			g_saucer.isLive = false;
			g_saucer.deathTime = g_time;
			if (g_saucer.mourningTimer > 0)
				g_saucer.mourningTimer -= 1;
			if (g_saucer.size == SMALL && g_saucer.inaccuracy != 0)
				g_saucer.inaccuracy += (g_saucer.inaccuracy < 0) ? 0.5F : -0.5F;
		}
		if ((g_time - g_saucer.shootTime > 0.66)
			&& (g_saucer.hitBox.center.x < SC_W - g_saucer.hitBox.radius * 2)
			&& (g_saucer.hitBox.center.x > g_saucer.hitBox.radius * 2)
			&& (g_player.isLive || g_gameState.currentScreen == POSTGAME))
		{
			saucerShoot();
		}
		switch (g_saucer.size)
		{
			case BIG:
				if (!IsSoundPlaying(g_sounds.saucerBg))
					PlaySound(g_sounds.saucerBg);
				break;
			case SMALL:
				if (!IsSoundPlaying(g_sounds.saucerSm))
					PlaySound(g_sounds.saucerSm);
				break;
			default:
				break;
		}
	}
}

void	updateSaucerBullets(void)
{
	if (g_saucer.bullets[0].isLive)
	{
		g_saucer.bullets[0].pos = Vector2Add(
			g_saucer.bullets[0].pos,
			Vector2Scale(g_saucer.bullets[0].vel, g_frameTime)
		);
		if (g_saucer.bullets[0].pos.x > SC_W)
			g_saucer.bullets[0].pos.x = 0;
		if (g_saucer.bullets[0].pos.y > SC_H)
			g_saucer.bullets[0].pos.y = 0;
		if (g_saucer.bullets[0].pos.x < 0)
			g_saucer.bullets[0].pos.x = SC_W;
		if (g_saucer.bullets[0].pos.y < 0)
			g_saucer.bullets[0].pos.y = SC_H;
		g_saucer.bullets[0].ttl -= g_frameTime;
		if (g_saucer.bullets[0].ttl <= 0.0F)
			g_saucer.bullets[0].isLive = false;
	}
	if (g_saucer.bullets[1].isLive)
	{
		g_saucer.bullets[1].pos = Vector2Add(
			g_saucer.bullets[1].pos,
			Vector2Scale(g_saucer.bullets[1].vel, g_frameTime)
		);
		if (g_saucer.bullets[1].pos.x > SC_W)
			g_saucer.bullets[1].pos.x = 0;
		if (g_saucer.bullets[1].pos.y > SC_H)
			g_saucer.bullets[1].pos.y = 0;
		if (g_saucer.bullets[1].pos.x < 0)
			g_saucer.bullets[1].pos.x = SC_W;
		if (g_saucer.bullets[1].pos.y < 0)
			g_saucer.bullets[1].pos.y = SC_H;
		g_saucer.bullets[1].ttl -= g_frameTime;
		if (g_saucer.bullets[1].ttl <= 0.0F)
			g_saucer.bullets[1].isLive = false;
	}
}

void	drawSaucer(void)
{
	if (g_saucer.isLive)
	{
		if (g_saucer.size == BIG)
			DrawTexture(
				g_saucer.textureBg,
				g_saucer.pos.x,
				g_saucer.pos.y,
				WHITE
			);
		else 
			DrawTexture(
				g_saucer.textureSm,
				g_saucer.pos.x,
				g_saucer.pos.y,
				WHITE
			);
		//DrawCircleLinesV(g_saucer.hitBox.center, g_saucer.hitBox.radius, RED);
	}
}

void	drawSaucerBullets(void)
{
	if (g_saucer.bullets[0].isLive)
		DrawPixelV(g_saucer.bullets[0].pos, WHITE);
	if (g_saucer.bullets[1].isLive)
		DrawPixelV(g_saucer.bullets[1].pos, WHITE);
}
