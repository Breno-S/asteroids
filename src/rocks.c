#include "explosions.h"
#include "gameState.h"
#include "main.h"
#include "playerShip.h"
#include "raylib.h"
#include "raymath.h"
#include "resources.h"
#include "rocks.h"
#include "saucer.h"

t_Rock	g_rocks[ROCK_MAX];

void	spawnRocks(void)
{
	static struct {unsigned n: 2;}	idx = {0};
	static const Rectangle			spawnAreas[4] = {
		{0, -10, SC_W, 10},
		{SC_W - 10, 0, SC_W + 10, SC_H},
		{0, SC_H - 10, SC_W, SC_H + 10},
		{-10, 0, 10, SC_H}
	};

	if ((g_time - g_gameState.cleanTime > 3)
		&& (!g_saucer.isLive)
		&& (g_time - g_saucer.deathTime > 2)
		&& (g_player.isLive || (g_gameState.currentScreen == POSTGAME))
		&& (g_time - g_player.spawnTime > 0.5))
	{
		for (int i = 0; i < g_gameState.rocksToSpawn * 4; i += 4)
		{
			g_rocks[i] = (t_Rock){
				.hitBox.center.x = GetRandomValue(
					spawnAreas[idx.n].x,
					spawnAreas[idx.n].width
				),
				.hitBox.center.y = GetRandomValue(
					spawnAreas[idx.n].y,
					spawnAreas[idx.n].height
				),
				.hitBox.radius = 20,
				.textureIdx = GetRandomValue(0, 3),
				.size = BIG,
				.vel.x = GetRandomValue(35, 40),
				.isLive = true
			};
			g_rocks[i].pos.x = g_rocks[i].hitBox.center.x - 20;
			g_rocks[i].pos.y = g_rocks[i].hitBox.center.y - 20;
			g_rocks[i].vel = Vector2Rotate(
				g_rocks[i].vel,
				GetRandomValue(0, 360) * DEG2RAD
			);
			idx.n++;
			g_gameState.rockCount++;
		}
		if (g_gameState.rocksToSpawn + 2 < ROCK_SPAWN_MAX)
			g_gameState.rocksToSpawn += 2;
		else
			g_gameState.rocksToSpawn = ROCK_SPAWN_MAX;
		g_gameState.levelStartTime = g_time;
	}
}

static void	deflectRockVelocity(t_Rock *rock)
{
	rock->vel = Vector2Scale(rock->vel, 0.25 * GetRandomValue(4, 8));
	rock->vel = Vector2Rotate(rock->vel, GetRandomValue(-90, 90) * DEG2RAD);
}

void	decayRock(unsigned short rockIdx, bool	shouldScore)
{
	explodeAt(g_rocks[rockIdx].hitBox.center);
	switch (g_rocks[rockIdx].size)
	{
		case BIG:
			g_rocks[rockIdx].size = MEDIUM;
			g_rocks[rockIdx].pos.x += 10;
			g_rocks[rockIdx].pos.y += 10;
			g_rocks[rockIdx].hitBox.radius /= 2;
			g_rocks[rockIdx + 2] = g_rocks[rockIdx];
			g_rocks[rockIdx].textureIdx = GetRandomValue(0, 3);
			g_rocks[rockIdx + 2].textureIdx = GetRandomValue(0, 3);
			deflectRockVelocity(&g_rocks[rockIdx]);
			deflectRockVelocity(&g_rocks[rockIdx + 2]);
			if (shouldScore)
				scoreAdd(20);
			g_gameState.rockCount++;
			PlaySound(g_sounds.bangBgSFX);
			break;
		case MEDIUM:
			g_rocks[rockIdx].size = SMALL;
			g_rocks[rockIdx].pos.x += 5;
			g_rocks[rockIdx].pos.y += 5;
			g_rocks[rockIdx].hitBox.radius /= 2;
			g_rocks[rockIdx + 1] = g_rocks[rockIdx];
			g_rocks[rockIdx].textureIdx = GetRandomValue(0, 3);
			g_rocks[rockIdx + 1].textureIdx = GetRandomValue(0, 3);
			deflectRockVelocity(&g_rocks[rockIdx]);
			deflectRockVelocity(&g_rocks[rockIdx + 1]);
			if (shouldScore)
				scoreAdd(50);
			g_gameState.rockCount++;
			PlaySound(g_sounds.bangMdSFX);
			break;
		case SMALL:
			g_rocks[rockIdx].isLive = false;
			if (shouldScore)
				scoreAdd(100);
			g_gameState.rockCount--;
			if (g_gameState.rockCount == 0)
			{
				g_gameState.cleanTime = g_time;
				g_music.rest = 1.0;
			}
			PlaySound(g_sounds.bangSmSFX);
			break;
	}
}

void	updateRocks(void)
{
	for (int i = 0; i < ROCK_MAX; i++)
	{
		g_rocks[i].pos = Vector2Add(
			g_rocks[i].pos,
			Vector2Scale(g_rocks[i].vel, g_frameTime)
		);
		g_rocks[i].hitBox.center = Vector2Add(
			g_rocks[i].hitBox.center,
			Vector2Scale(g_rocks[i].vel, g_frameTime)
		);
		if (g_rocks[i].hitBox.center.x > SC_W)
		{
			g_rocks[i].pos.x = 0 - g_rocks[i].hitBox.radius;
			g_rocks[i].hitBox.center.x = 0;
		}
		if (g_rocks[i].hitBox.center.x < 0)
		{
			g_rocks[i].pos.x = SC_W - g_rocks[i].hitBox.radius;
			g_rocks[i].hitBox.center.x = SC_W;
		}
		if (g_rocks[i].hitBox.center.y > SC_H)
		{
			g_rocks[i].pos.y = 0 - g_rocks[i].hitBox.radius;
			g_rocks[i].hitBox.center.y = 0;
		}
		if (g_rocks[i].hitBox.center.y < 0)
		{
			g_rocks[i].pos.y = SC_H - g_rocks[i].hitBox.radius;
			g_rocks[i].hitBox.center.y = SC_H;
		}
	}
}

void	drawRocks(void)
{
	for (int i = 0; i < ROCK_MAX; i++)
	{
		if (g_rocks[i].isLive)
		{
			DrawTexture(
				g_rockTextures[g_rocks[i].textureIdx + 4 * g_rocks[i].size],
				g_rocks[i].pos.x,
				g_rocks[i].pos.y,
				WHITE
			);
			//char	stats[32] = {0};
			//DrawCircleLinesV(g_rocks[i].hitBox.center, g_rocks[i].hitBox.radius, RED);
			//sprintf(stats, "%.2f", Vector2Length(g_rocks[i].vel)); 
			//DrawText(stats, g_rocks[i].pos.x, g_rocks[i].pos.y + 30, 10, YELLOW);
		}
	}
}
