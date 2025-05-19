#include "gameState.h"
#include "highScore.h"
#include "main.h"
#include "raylib.h"
#include "records.h"
#include "resources.h"

static char	g_initials[3] = {'_', '_', '_'};

void	handleHighScoreInput(void)
{
	static struct {unsigned int n: 2;} charIdx = {0};

	if (charIdx.n == 3)
	{
		saveRecord(g_initials, g_gameState.score);
		loadRecords(g_localRecords);
		charIdx.n = 0;
		g_initials[0] = '_';
		g_initials[1] = '_';
		g_initials[2] = '_';
		g_gameState.currentScreen = POSTGAME;
	}
	else
	{
		if (IsKeyPressed(KEY_D))
		{
			switch (g_initials[charIdx.n])
			{
				case 'Z':
					g_initials[charIdx.n] = '_';
					break;
				case '_':
					g_initials[charIdx.n] = 'A';
					break;
				default:
					g_initials[charIdx.n]++;
			}
		}
		if (IsKeyPressed(KEY_A))
		{
			switch (g_initials[charIdx.n])
			{
				case 'A':
					g_initials[charIdx.n] = '_';
					break;
				case '_':
					g_initials[charIdx.n] = 'Z';
					break;
				default:
					g_initials[charIdx.n]--;
			}
		}
		if (IsKeyPressed(KEY_K))
			charIdx.n++;
	}
}

void	drawHighScoreScreen(void)
{
	static const char	msg[] = "YOUR SCORE IS ONE OF THE TEN BEST\n"
		"PLEASE ENTER YOUR INITIALS\n"
		"PUSH ROTATE TO SELECT LETTER\n"
		"PUSH HYPERSPACE WHEN LETTER IS CORRECT";

	drawScore();
	drawHighScore();
	SetTextLineSpacing(-8);
	DrawTextEx(g_fontBold32, msg, (Vector2){30, SC_H / 2 - 100}, 32, 0, GRAY);
	DrawTextCodepoint(g_font64, g_initials[0], (Vector2){269, 350}, 64, GRAY);
	DrawTextCodepoint(g_font64, g_initials[1], (Vector2){307, 350}, 64, GRAY);
	DrawTextCodepoint(g_font64, g_initials[2], (Vector2){343, 350}, 64, GRAY);
}
