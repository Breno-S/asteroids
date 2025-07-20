#include <stdbool.h>
#include <stdio.h>

#include "explosions.h"
#include "gameState.h"
#include "main.h"
#include "postGame.h"
#include "raylib.h"
#include "records.h"
#include "resources.h"
#include "rocks.h"
#include "saucer.h"

static double	g_postGameTime = 1;
static bool		g_shouldDemo = false;

void	handlePostGameInput(void)
{
	if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))
	{
		startSession();
		g_shouldDemo = false;
		g_postGameTime = 1;
	}
}

void	updatePostGame(void)
{
	g_postGameTime += g_frameTime;
	if ((int)g_postGameTime % 16 == 0)
	{
		g_shouldDemo = !g_shouldDemo;
		g_postGameTime = 1;
	}
	if (g_shouldDemo)
	{
		updateRocks();
		updateSaucer();
		updateSaucerBullets();
		updateExplosions();
		if (g_saucer.isLive)
			handleSaucerCollisions();
		else
			requestSaucer();
		handleSaucerBulletCollisions();
		if (g_gameState.rockCount == 0)
			spawnRocks();
	}
}

static void	drawRanking(void)
{
	char			row[20] = {0};
	unsigned short	scoreWidth = 0;
	unsigned short	rowWidth = 0;
	unsigned int	highest = g_localRecords[0].score;

	while (highest > 0)
	{
		highest /= 10;
		scoreWidth++;
	}
	DrawTextEx(g_fontBold32, "HIGH SCORES", (Vector2){SC_W / 2 - 85, 110}, 32, 0, WHITE);
	for (int i = 0; i < 10; i++)
	{
		if (g_localRecords[i].score > 0)
		{
			rowWidth = sprintf(
				row,
				"% 3d. %*.2u %c%c%c",
				i + 1,
				scoreWidth,
				g_localRecords[i].score,
				g_localRecords[i].initials[0],
				g_localRecords[i].initials[1],
				g_localRecords[i].initials[2]
			);
			DrawTextEx(
				g_fontBold32,
				row,
				(Vector2){SC_W / 2 - 16 - (16 * rowWidth / 2), 150 + 24 * i},
				32,
				0,
				WHITE
			);
		}
	}
}

void	drawPostGameScreen(void)
{
	if (g_shouldDemo)
	{
		drawRocks();
		drawSaucer();
		drawSaucerBullets();
		drawExplosions();
	}
	else
		drawRanking();
	drawScore();
	drawHighScore();
	if ((int)g_time % 2 == 0)
		DrawTextEx(g_fontBold32, "PUSH ENTER", (Vector2){SC_W / 2 - 77, 60}, 32, 0, WHITE);
}
