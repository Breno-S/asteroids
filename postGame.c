#include "raylib.h"
#include "gameObjects.h"
#include "resources.h"
#include "explosions.h"
#include <stdio.h>
#include "postGame.h"

extern Font			fontBold32;
extern t_GameState	gameState;
extern t_Saucer		saucer;
extern t_Record		localRecords[10];
extern double		time;
extern float		frameTime;

double	postGameTime = 1;
bool	shouldDemo = false;

void	handlePostGameInput()
{
	if (IsKeyPressed(KEY_ENTER))
	{
		initSession();
		shouldDemo = false;
		postGameTime = 1;
	}
}

void	updatePostGame()
{
	postGameTime += frameTime;
	if ((int)postGameTime % 16 == 0)
	{
		shouldDemo = !shouldDemo;
		postGameTime = 1;
	}
	if (shouldDemo)
	{
		updateRocks();
		updateSaucer();
		updateSaucerBullets();
		updateExplosions();
		if (saucer.isLive)
			handleSaucerCollisions();
		else
			requestSaucer();
		handleSaucerBulletCollisions();
		if (gameState.rockCount == 0)
			spawnRocks();
	}
}

static void	drawRanking()
{
	char			row[20] = { 0 };
	unsigned short	scoreWidth = 0;
	unsigned short	rowWidth = 0;
	unsigned int	highest = localRecords[0].score;

	while (highest > 0)
	{
		highest /= 10;
		scoreWidth++; 
	}
	DrawTextEx(fontBold32, "HIGH SCORES", (Vector2){SC_W / 2 - 85, 110}, 32, 0, GRAY);
	for (int i = 0; i < 10; i++)
	{
		if (localRecords[i].score > 0)
		{
			rowWidth = sprintf(
				row,
				"% 3d. %*.2u %c%c%c",
				i + 1,
				scoreWidth,
				localRecords[i].score,
				localRecords[i].initials[0],
				localRecords[i].initials[1],
				localRecords[i].initials[2]
			);
			DrawTextEx(fontBold32, row, (Vector2){SC_W / 2 - 16 - (16 * rowWidth / 2), 150 + 24 * i}, 32, 0, GRAY);
		}
	}
}

void	drawPostGameScreen()
{
	if (shouldDemo)
	{
		drawRocks();
		drawSaucer();
		drawSaucerBullets();
		drawExplosions();
	}
	else
	{
		drawRanking();
	}
	drawScore();
	drawHighScore();
	if ((int)time % 2 == 0)
		DrawTextEx(fontBold32, "PUSH ENTER", (Vector2){SC_W / 2 - 77, 60}, 32, 0, GRAY);
}
