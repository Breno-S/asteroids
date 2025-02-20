#include "raylib.h"
#include "resources.h"
#include "gameObjects.h"
#include "title.h"
#include "game.h"
#include "highScore.h"
#include "postGame.h"
#include "explosions.h"
#include "header.h"

/***************************** GLOBAL VARIABLES *******************************/

extern t_GameState	gameState;

double	time;
float	frameTime;

/************************************ MAIN ************************************/

int	main(void)
{
	InitWindow(SC_W, SC_H, "Asteroids");
	SetTargetFPS(60);
	InitAudioDevice();
	loadAllTextures();
	loadAllFonts();
	loadAllSounds();

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

	CloseAudioDevice();
	CloseWindow();
	return (0);
}
