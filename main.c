#include "raylib.h"
#include "resources.h"
#include "gameObjects.h"
#include "title.h"
#include "game.h"
#include "highScore.h"
#include "postGame.h"
#include "header.h"

double	time;
float	frameTime;

int	main(void)
{
	InitWindow(SC_W, SC_H, "Asteroids");
	SetTargetFPS(60);
	InitAudioDevice();
	loadAllResources();

	while (!WindowShouldClose())
	{
		frameTime = GetFrameTime();
		if (!gameState.isPaused)
			time += frameTime;
		switch (gameState.currentScreen)
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
			switch (gameState.currentScreen)
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
