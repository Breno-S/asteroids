#include "explosions.h"
#include "main.h"
#include "raylib.h"
#include "raymath.h"

#define EXPLOSION_MAX 20

static t_Explosion	g_explosion[EXPLOSION_MAX];

void	explodeAt(Vector2 place)
{
	static unsigned short	explosionIdx = 0;

	for (int i = 0; i < EXPLOSION_DEBRIS; i++)
	{
		g_explosion[explosionIdx].debris[i].pos = place;
		g_explosion[explosionIdx].debris[i].vel.x = GetRandomValue(40, 50);
		g_explosion[explosionIdx].debris[i].vel = Vector2Rotate(
			g_explosion[explosionIdx].debris[i].vel,
			GetRandomValue(0, 360) * DEG2RAD
		);
		g_explosion[explosionIdx].isLive = true;
		g_explosion[explosionIdx].startTime = GetTime();
	}
	explosionIdx++;
	if (explosionIdx == EXPLOSION_MAX)
		explosionIdx = 0;
}

void	updateExplosions(void)
{
	for (int i = 0; i < EXPLOSION_MAX; i++)
	{
		if (g_explosion[i].isLive)
		{
			for (int j = 0; j < EXPLOSION_DEBRIS; j++)
			{
				g_explosion[i].debris[j].pos = Vector2Add(
					g_explosion[i].debris[j].pos,
					Vector2Scale(g_explosion[i].debris[j].vel, g_frameTime)
				);
				if (GetTime() - g_explosion[i].startTime > 0.5)
					g_explosion[i].isLive = false;
			}
		}
	}
}

void	drawExplosions(void)
{
	for (int i = 0; i < EXPLOSION_MAX; i++)
	{
		if (g_explosion[i].isLive)
		{
			for (int j = 0; j < EXPLOSION_DEBRIS; j++)
				DrawPixelV(g_explosion[i].debris[j].pos, GRAY);
		}
	}
}
