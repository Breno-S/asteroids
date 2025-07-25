#include "raylib.h"
#include "main.h"
#include "gameState.h"
#include "title.h"

void	handleTitleInput(void)
{
	if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))
		startSession();
}

void	drawTitleScreen(void)
{
	static unsigned char	alpha = 40;
	static unsigned char	dAlpha = 8;

	DrawText("ASTEROIDS", 35, SC_H / 2 - 56, 96, WHITE);
	DrawText("Press ENTER to play", SC_W / 2 - 150, SC_H - 108, 28, (Color){255, 255, 0, alpha});
	alpha += dAlpha;
	if (alpha == 32)
		dAlpha *= -1;
	if (alpha == 248)
		dAlpha *= -1;
}
