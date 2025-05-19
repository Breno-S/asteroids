#include "game.h"
#include "gameState.h"
#include "highScore.h"
#include "main.h"
#include "postGame.h"
#include "raylib.h"
#include "resources.h"
#include "title.h"

double	g_time;
float	g_frameTime;

int	main(void)
{
	InitWindow(SC_W, SC_H, "Asteroids");
	SetTargetFPS(60);
	InitAudioDevice();
	loadAllResources();
	SetWindowIcon(g_windowIcon);

	while (!WindowShouldClose())
	{
		g_frameTime = GetFrameTime();
		if (!g_gameState.isPaused)
			g_time += g_frameTime;
		switch (g_gameState.currentScreen)
		{
			case TITLE:
				handleTitleInput();
				break;
			case GAME:
				handleGameInput();
				updateGame();
				break;
			case HIGHSCORE:
				handleHighScoreInput();
				break;
			case POSTGAME:
				handlePostGameInput();
				updatePostGame();
				break;
		}

		BeginDrawing();
			ClearBackground(BLACK);
			switch (g_gameState.currentScreen)
			{
				case TITLE:
					drawTitleScreen();
					break;
				case GAME:
					drawGameScreen();
					break;
				case HIGHSCORE:
					drawHighScoreScreen();
					break;
				case POSTGAME:
					drawPostGameScreen();
					break;
			};
		EndDrawing();
	}

	unloadAllResources();
	CloseAudioDevice();
	CloseWindow();
	return (0);
}
