#include <stdio.h>

#include "gameState.h"
#include "main.h"
#include "playerShip.h"
#include "raylib.h"
#include "records.h"
#include "resources.h"
#include "rocks.h"
#include "saucer.h"

t_GameState	g_gameState = {
	.currentScreen = TITLE
};

void	startSession(void)
{
	SetMasterVolume(0.25);
	loadRecords(g_localRecords);
	for (int i = 0; i < ROCK_MAX; i++)
		g_rocks[i].isLive = 0;
	g_gameState.rockCount = 0;
	g_gameState.rocksToSpawn = 4;
	g_gameState.numLives = 3;
	g_gameState.score = 0;
	g_gameState.oneUpMeter = 0;
	g_gameState.cleanTime = g_time - 2;
	g_gameState.currentScreen = GAME;
	g_gameState.isGameOver = false;
	g_player.angle = 0;
	g_player.deathTime = g_time - 2;
	g_saucer.isLive = false;
	g_saucer.bullets[0].isLive = false;
	g_saucer.bullets[1].isLive = false;
	g_saucer.mourningTimer = 11;
	g_saucer.shouldRespawn = false;
	g_saucer.requestTime = 0;
	g_saucer.inaccuracy = 10.0F;
	g_music.rest = 1.0;
}

void	endSession(void)
{
	SetMasterVolume(0);
	for (int i = 0; i < 10; i++)
	{
		if (g_gameState.score > g_localRecords[i].score)
		{
			g_gameState.currentScreen = HIGHSCORE;
			return;
		}
	}
	g_gameState.currentScreen = POSTGAME;
}

void	scoreAdd(unsigned short points)
{
	g_gameState.score += points;
	g_gameState.oneUpMeter += points;
	if (g_gameState.oneUpMeter > 10000)
	{
		g_gameState.numLives++;
		g_gameState.oneUpMeter -= 10000;
		PlaySound(g_sounds.extraShipSFX);
	}
}

void	drawLives(void)
{
	for (int i = 0; i < g_gameState.numLives; i++)
	{
		DrawTexturePro(
			g_player.texture,
			(Rectangle){0, 0, 21, 10},
			(Rectangle){86 + 10 * i, 42, g_player.currSprite.width, g_player.currSprite.height},
			g_player.CoM,
			-90,
			(Color){255, 255, 255, 127}
		);
	}
}

void	drawScore(void)
{
	static char	scoreStr[15];

	sprintf(scoreStr, "%8.2u", g_gameState.score);
	DrawTextEx(g_fontBold32, scoreStr, (Vector2){2, 0}, 32, 0, (Color){255, 255, 255, 127});
}
