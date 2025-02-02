#include "explosions.h"
#include "raylib.h"
#include "raymath.h"

t_Explosion		explosion[EXPLOSION_MAX]; 
extern float	frameTime;

void	explodeAt(Vector2 place)
{
	static unsigned short	explosionIdx = 0;

	for (int i = 0; i < EXPLOSION_DEBRIS; i++)
	{
		explosion[explosionIdx].debris[i].pos = place;
		explosion[explosionIdx].debris[i].vel.x = GetRandomValue(40, 50);
		explosion[explosionIdx].debris[i].vel = Vector2Rotate(explosion[explosionIdx].debris[i].vel, GetRandomValue(0, 360) * DEG2RAD);
		explosion[explosionIdx].isLive = true;
		explosion[explosionIdx].startTime = GetTime();
	}
	explosionIdx++;
	if (explosionIdx == EXPLOSION_MAX)
		explosionIdx = 0;
}

void	updateExplosions()
{
	for (int i = 0; i < EXPLOSION_MAX; i++)
	{
		if (explosion[i].isLive)
		{
			for (int j = 0; j < EXPLOSION_DEBRIS; j++)
			{
				explosion[i].debris[j].pos = Vector2Add(explosion[i].debris[j].pos, Vector2Scale(explosion[i].debris[j].vel, frameTime));
				if (GetTime() - explosion[i].startTime > 0.5)
					explosion[i].isLive = false;
			}
		}
	}
}

void	drawExplosions()
{
	for (int i = 0; i < EXPLOSION_MAX; i++)
	{
		if (explosion[i].isLive)
		{
			for (int j = 0; j < EXPLOSION_DEBRIS; j++)
			{
				DrawPixelV(explosion[i].debris[j].pos, GRAY);
			}
		}
	}
}
