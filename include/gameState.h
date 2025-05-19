#ifndef GAMESTATE_H
# define GAMESTATE_H

# include <stdbool.h>

# include "main.h"

typedef struct s_GameState
{
	unsigned int		score;
	double				levelStartTime;
	double				cleanTime;
	unsigned char		numLives;
	unsigned short		oneUpMeter;
	unsigned short		rockCount;
	unsigned short		rocksToSpawn;
	enum e_GameScreen	currentScreen;
	bool				isPaused;
	bool				isGameOver;
}	t_GameState;

void	startSession(void);
void	endSession(void);
void	scoreAdd(unsigned short points);
void	drawLives(void);
void	drawScore(void);

extern t_GameState	g_gameState;

#endif
