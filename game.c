#include "raylib.h"
#include "gameObjects.h"
#include "resources.h"
#include "explosions.h"
#include "game.h"

extern t_Sounds		sounds;
extern Font			fontBold32;
extern t_GameState	gameState;
extern t_PlayerShip	player;
extern t_Saucer		saucer;
extern double		time;
extern float		frameTime;

void	handleGameInput()
{
	if (!gameState.isPaused)
	{
		if (player.isLive && !player.inHyperspace)
		{
			if (IsKeyDown(KEY_D))
				player.angle += 240 * frameTime;
			if (IsKeyDown(KEY_A))
				player.angle -= 240 * frameTime;
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
		gameState.isPaused = !gameState.isPaused;
		StopSound(sounds.thrust);
	}
}

void	updateGame()
{
	if (!gameState.isPaused)
	{
		if (player.isLive)
		{
			playGameMusic();
			if (player.inHyperspace)
				exitHyperspace();
			else
				handlePlayerCollisions();
		}
		else
		{
			if (gameState.numLives > 0)
				playerRespawn();
			else
				gameState.isGameOver = true;
		}
		if (saucer.isLive)
			handleSaucerCollisions();
		else
			requestSaucer();
		handlePlayerBulletCollisions();
		handleSaucerBulletCollisions();
		if (gameState.oneUpMeter >= 10000)
		{
			gameState.numLives++;
			gameState.oneUpMeter -= 10000;
		}
		if (gameState.rockCount == 0)
			spawnRocks();
		if (!gameState.isGameOver)
			updatePlayer();
		else
		{
			if (time - player.deathTime > 5)
				endSession();
		}
		updateSaucer();
		updatePlayerBullets();
		updateSaucerBullets();
		updateRocks();
		updateExplosions();
	}
}

void	drawGameScreen()
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
	if (gameState.isGameOver)
		DrawTextEx(fontBold32, "GAME OVER", (Vector2){SC_W/2 - 74, SC_H/2 - 100}, 32, 1, (Color){127, 127, 127, 255});
}
