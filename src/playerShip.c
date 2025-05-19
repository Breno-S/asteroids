#include <stdbool.h>

#include "explosions.h"
#include "gameState.h"
#include "main.h"
#include "playerShip.h"
#include "postGame.h"
#include "raylib.h"
#include "raymath.h"
#include "resources.h"
#include "rocks.h"
#include "saucer.h"

t_PlayerShip	g_player = {
	.currSprite = {0, 0, 21, 10},
	.hitBox = {{SC_W / 2, SC_H / 2}, 5},
	.CoM = {10, 5},
	.pos = {SC_W / 2, SC_H / 2},
	.maxSpeed = 300,
	.accel = 200,
};

void	decelerate(void)
{
	if (Vector2Length(g_player.vel) > 0)
		g_player.vel = Vector2Subtract(
			g_player.vel,
			Vector2Scale(g_player.vel, 0.25 * g_frameTime)
		);
	g_player.currSprite.x = 0;
	g_sounds.thrustVolume -= 0.1F;
	if (g_sounds.thrustVolume < 0.0F)
		g_sounds.thrustVolume = 0.0F;
	SetSoundVolume(g_sounds.thrust, g_sounds.thrustVolume);
}

void	accelerate(void)
{
	float	speed;
	float	scale;

	g_player.vel.x += g_player.accel * cosf(g_player.angle * DEG2RAD) * g_frameTime;
	g_player.vel.y += g_player.accel * sinf(g_player.angle * DEG2RAD) * g_frameTime;
	speed = Vector2Length((Vector2){g_player.vel.x, g_player.vel.y});
	if (speed > g_player.maxSpeed)
	{
		scale = g_player.maxSpeed / speed;
		g_player.vel.x *= scale;
		g_player.vel.y *= scale;
	}
	g_player.currSprite.x = 21;
	g_sounds.thrustVolume += 0.1F;
	if (g_sounds.thrustVolume > 1.0F)
		g_sounds.thrustVolume = 1.0F;
	SetSoundVolume(g_sounds.thrust, g_sounds.thrustVolume);
	if (!IsSoundPlaying(g_sounds.thrust))
		PlaySound(g_sounds.thrust);
}

void	playerShoot(void)
{
	static unsigned short	bulletIdx = 0;
	bool					isFull = true;
	float					bulletSpeed = g_player.maxSpeed;

	for (int i = 0; i < BULLET_MAX; i++)
		if (g_player.bulletPool[i].isLive == false)
			isFull = false;
	if (!isFull && !g_player.inHyperspace)
	{
		g_player.bulletPool[bulletIdx].pos = (Vector2){
			.x = g_player.pos.x + 8 * cosf(g_player.angle * DEG2RAD),
			.y = g_player.pos.y + 8 * sinf(g_player.angle * DEG2RAD)
		};
		g_player.bulletPool[bulletIdx].vel = (Vector2){
			.x = bulletSpeed * cosf(g_player.angle * DEG2RAD) + g_player.vel.x,
			.y = bulletSpeed * sinf(g_player.angle * DEG2RAD) + g_player.vel.y
		};
		g_player.bulletPool[bulletIdx].ttl = 1.3;
		g_player.bulletPool[bulletIdx].isLive = true;
		bulletIdx++;
		if (bulletIdx == BULLET_MAX)
			bulletIdx = 0;
		PlaySound(g_sounds.fireSFX);
	}
}

void	playerDie(void)
{
	explodeAt(g_player.hitBox.center);
	g_player.isLive = false;
	g_gameState.numLives--;
	g_player.deathTime = g_time;
	StopSound(g_sounds.thrust);
	PlaySound(g_sounds.bangMdSFX);
}

void	playerRespawn(void)
{
	for (int i = 0; i < ROCK_MAX; i++)
	{
		if (g_rocks[i].isLive)
			if (CheckCollisionCircleRec(
					g_rocks[i].hitBox.center,
					g_rocks[i].hitBox.radius,
					(Rectangle){SC_W / 2 - 50, SC_H / 2 - 50, 100, 100}))
				return;
	}
	if (g_time - g_player.deathTime > 3)
	{
		if (g_saucer.isLive)
		{
			g_saucer.bullets[0].isLive = false;
			g_saucer.bullets[1].isLive = false;
			g_saucer.isLive = false;
			g_saucer.deathTime = g_time;
		}
		g_player.vel.x = 0;
		g_player.vel.y = 0;
		g_player.pos.x = SC_W / 2;
		g_player.pos.y = SC_H / 2;
		g_player.hitBox.center.x = SC_W / 2;
		g_player.hitBox.center.y = SC_H / 2;
		g_player.isLive = true;
		g_player.spawnTime = g_time;
	}
}

void	enterHyperspace(void)
{
	StopSound(g_sounds.thrust);
	g_player.hyperspaceTime = g_time;
	g_player.inHyperspace = true;
}

void	exitHyperspace(void)
{
	if (g_time - g_player.hyperspaceTime > 1.5)
	{
		g_player.vel.x = 0;
		g_player.vel.y = 0;
		g_player.pos.x = GetRandomValue(40, 600);
		g_player.pos.y = GetRandomValue(40, 440);
		g_player.hitBox.center.x = g_player.pos.x;
		g_player.hitBox.center.y = g_player.pos.y;
		g_player.inHyperspace = false;
		if (!GetRandomValue(0, 4))
			playerDie();
	}
}

void	updatePlayer(void)
{
	if (g_player.isLive)
	{
		g_player.pos.x += g_player.vel.x * g_frameTime;
		g_player.pos.y += g_player.vel.y * g_frameTime;
		g_player.hitBox.center.x += g_player.vel.x * g_frameTime;
		g_player.hitBox.center.y += g_player.vel.y * g_frameTime;
		if (g_player.pos.x > SC_W)
		{
			g_player.pos.x = 0;
			g_player.hitBox.center.x = 0;
		}
		if (g_player.pos.y > SC_H)
		{
			g_player.pos.y = 0;
			g_player.hitBox.center.y = 0;
		}
		if (g_player.pos.x < 0)
		{
			g_player.pos.x = SC_W;
			g_player.hitBox.center.x = SC_W;
		}
		if (g_player.pos.y < 0)
		{
			g_player.pos.y = SC_H;
			g_player.hitBox.center.y = SC_H;
		}
	}
}

void	handlePlayerCollisions(void)
{
	for (int i = 0; i < ROCK_MAX; i++)
	{
		if (g_rocks[i].isLive && g_player.isLive)
		{
			if (CheckCollisionCircles(
					g_player.hitBox.center,
					g_player.hitBox.radius,
					g_rocks[i].hitBox.center,
					g_rocks[i].hitBox.radius))
			{
				playerDie();
				decayRock(i, true);
			}
		}
	}
	if (g_saucer.isLive)
	{
		if (CheckCollisionCircles(
				g_player.hitBox.center,
				g_player.hitBox.radius,
				g_saucer.hitBox.center,
				g_saucer.hitBox.radius))
		{
			playerDie();
			saucerDie();
			if (g_saucer.size == BIG)
				scoreAdd(200);
			else
				scoreAdd(1000);
		}
	}
	if (g_saucer.bullets[0].isLive)
	{
		if (CheckCollisionPointCircle(
				g_saucer.bullets[0].pos,
				g_player.hitBox.center,
				g_player.hitBox.radius))
		{
			playerDie();
			g_saucer.bullets[0].isLive = false;
		}
	}
	if (g_saucer.bullets[1].isLive)
	{
		if (CheckCollisionPointCircle(
				g_saucer.bullets[1].pos,
				g_player.hitBox.center,
				g_player.hitBox.radius))
		{
			playerDie();
			g_saucer.bullets[1].isLive = false;
		}
	}
}

void	handlePlayerBulletCollisions(void)
{
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_player.bulletPool[i].isLive)
		{
			for (int j = 0; j < ROCK_MAX; j++)
			{
				if (g_rocks[j].isLive)
				{
					if (CheckCollisionPointCircle(
							g_player.bulletPool[i].pos,
							g_rocks[j].hitBox.center,
							g_rocks[j].hitBox.radius))
					{
						g_player.bulletPool[i].isLive = false;
						decayRock(j, true);
					}
				}
			}
			if (g_saucer.isLive)
			{
				if (CheckCollisionPointCircle(
						g_player.bulletPool[i].pos,
						g_saucer.hitBox.center,
						g_saucer.hitBox.radius))
				{
					g_player.bulletPool[i].isLive = false;
					saucerDie();
					if (g_saucer.size == BIG)
						scoreAdd(200);
					else
						scoreAdd(1000);
				}
			}
		}
	}
}

void	updatePlayerBullets(void)
{
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_player.bulletPool[i].isLive)
		{
			g_player.bulletPool[i].pos = Vector2Add(
				g_player.bulletPool[i].pos,
				Vector2Scale(g_player.bulletPool[i].vel, g_frameTime)
			);
			if (g_player.bulletPool[i].pos.x > SC_W)
				g_player.bulletPool[i].pos.x = 0;
			if (g_player.bulletPool[i].pos.y > SC_H)
				g_player.bulletPool[i].pos.y = 0;
			if (g_player.bulletPool[i].pos.x < 0)
				g_player.bulletPool[i].pos.x = SC_W;
			if (g_player.bulletPool[i].pos.y < 0)
				g_player.bulletPool[i].pos.y = SC_H;
			g_player.bulletPool[i].ttl -= g_frameTime;
			if (g_player.bulletPool[i].ttl <= 0.0F)
				g_player.bulletPool[i].isLive = false;
		}
	}
}

void	drawPlayer(void)
{
	if (g_player.isLive && !g_player.inHyperspace)
	{
		DrawTexturePro(
			g_player.texture,
			g_player.currSprite,
			(Rectangle){g_player.pos.x, g_player.pos.y,
				g_player.currSprite.width, g_player.currSprite.height},
			g_player.CoM,
			g_player.angle,
			WHITE
		);
		// DrawCircleLinesV(g_player.hitBox.center, g_player.hitBox.radius, RED);
	}
}

void	drawPlayerBullets(void)
{
	for (int i = 0; i < BULLET_MAX; i++)
		if (g_player.bulletPool[i].isLive)
			DrawPixelV(g_player.bulletPool[i].pos, WHITE);
}
