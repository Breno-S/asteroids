#include "explosions.h"
#include "game.h"
#include "gameState.h"
#include "main.h"
#include "raylib.h"
#include "records.h"
#include "resources.h"
#include "rocks.h"
#include "playerShip.h"
#include "saucer.h"

void	handleGameInput(void)
{
	if (!g_gameState.isPaused)
	{
		if (g_player.isLive && !g_player.inHyperspace)
		{
			if (IsKeyDown(KEY_D))
				g_player.angle += 240 * g_frameTime;
			if (IsKeyDown(KEY_A))
				g_player.angle -= 240 * g_frameTime;
			if (IsKeyDown(KEY_W))
				accelerate();
			else
				decelerate();
			if (IsKeyPressed(KEY_J))
				playerShoot();
			if (IsKeyPressed(KEY_K))
				enterHyperspace();
		}
	}
	if (IsKeyPressed(KEY_SPACE))
	{
		g_gameState.isPaused = !g_gameState.isPaused;
		StopSound(g_sounds.thrust);
	}
}

void	updateGame(void)
{
	if (!g_gameState.isPaused)
	{
		if (g_player.isLive)
		{
			playGameMusic();
			if (g_player.inHyperspace)
				exitHyperspace();
			else
				handlePlayerCollisions();
		}
		else
		{
			if (g_gameState.numLives > 0)
				playerRespawn();
			else
				g_gameState.isGameOver = true;
		}
		if (g_saucer.isLive)
			handleSaucerCollisions();
		else
			requestSaucer();
		handlePlayerBulletCollisions();
		handleSaucerBulletCollisions();
		if (g_gameState.oneUpMeter >= 10000)
		{
			g_gameState.numLives++;
			g_gameState.oneUpMeter -= 10000;
		}
		if (g_gameState.rockCount == 0)
			spawnRocks();
		if (!g_gameState.isGameOver)
			updatePlayer();
		else
		{
			if (g_time - g_player.deathTime > 5)
				endSession();
		}
		updateSaucer();
		updatePlayerBullets();
		updateSaucerBullets();
		updateRocks();
		updateExplosions();
	}
}

void	drawGameScreen(void)
{
	drawPlayer();
	drawLives();
	drawPlayerBullets();
	drawSaucerBullets();
	drawRocks();
	drawSaucer();
	drawScore();
	drawExplosions();
	drawHighScore();
	if (g_gameState.isGameOver)
	{
		DrawTextEx(
			g_fontBold32,
			"GAME OVER",
			(Vector2){SC_W / 2 - 74, SC_H / 2 - 100},
			32,
			1,
			(Color){127, 127, 127, 255}
		);
	}
}
