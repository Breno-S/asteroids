#include "raylib.h"
#include "gameObjects.h"
#include "resources.h"
#include "header.h"
#include "highScore.h"

extern Font			fontBold32;
extern Font			font64;
extern t_GameState	gameState;
extern t_Record		localRecords[10];
extern double		time;

char	initials[3] = {'_', '_', '_'};

void	handleHighScoreInput()
{
	static struct {unsigned int n: 2;} charIdx = { 0 };

	if (charIdx.n == 3)
	{
		saveRecord(initials, gameState.score);
		loadRecords(localRecords);
		charIdx.n = 0;
		initials[0] = '_';
		initials[1] = '_';
		initials[2] = '_';
		gameState.currentScreen = POSTGAME;
	}
	else
	{
		if (IsKeyPressed(KEY_D))
		{
			switch (initials[charIdx.n])
			{
				case 'Z':
					initials[charIdx.n] = '_';
					break;
				case '_':
					initials[charIdx.n] = 'A';
					break;
				default:
					initials[charIdx.n]++;
			}
		}
		if (IsKeyPressed(KEY_A))
		{
			switch (initials[charIdx.n])
			{
				case 'A':
					initials[charIdx.n] = '_';
					break;
				case '_':
					initials[charIdx.n] = 'Z';
					break;
				default:
					initials[charIdx.n]--;
			}
		}
		if (IsKeyPressed(KEY_K))
		{
			charIdx.n++;
		}
	}
}

void	drawHighScoreScreen()
{
	static const char	msg[] = "YOUR SCORE IS ONE OF THE TEN BEST\n"
		"PLEASE ENTER YOUR INITIALS\n"
		"PUSH ROTATE TO SELECT LETTER\n"
		"PUSH HYPERSPACE WHEN LETTER IS CORRECT";

	drawScore();
	drawHighScore();
	SetTextLineSpacing(-8);
	DrawTextEx(fontBold32, msg, (Vector2){30, SC_H / 2 - 100}, 32, 0, GRAY);
	DrawTextCodepoint(font64, initials[0], (Vector2){269, 350}, 64, GRAY);
	DrawTextCodepoint(font64, initials[1], (Vector2){307, 350}, 64, GRAY);
	DrawTextCodepoint(font64, initials[2], (Vector2){343, 350}, 64, GRAY);
}
