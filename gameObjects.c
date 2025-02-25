#include "raylib.h"
#include "raymath.h"
#include "explosions.h"
#include "header.h"
#include "resources.h"
#include <stdio.h>
#include "gameObjects.h"

extern double	time;
extern float	frameTime;

t_GameState		gameState = {
	.currentScreen = TITLE
};
t_PlayerShip	player = {
	.currSprite = {0, 0, 21, 10},
	.hitBox = {{SC_W / 2, SC_H / 2}, 5},
	.CoM = {10, 5},
	.pos = {SC_W / 2, SC_H / 2},
	.maxSpeed = 300,
	.accel = 200,
};
t_Saucer		saucer = {
	.speed = 70,
	.inaccuracy = 10.0F,
	.mourningTimer = 10,
};
t_Rock			rockPool[ROCK_MAX];
t_Record		localRecords[10] = { 0 };	

// GAMESTATE

void	saveRecord(char *initials, unsigned int score)
{
	FILE			*fp;
	t_Record		r = {{initials[0], initials[1], initials[2]}, score};
	unsigned short	numRecords;

	if (r.initials[0] == '_') r.initials[0] = ' ';
	if (r.initials[1] == '_') r.initials[1] = ' ';
	if (r.initials[2] == '_') r.initials[2] = ' ';
	numRecords = loadRecords(localRecords);
	for (int i = 0; i <= numRecords; i++)
	{
		if (localRecords[i].score < score)
		{
			for (int j = 9; j > i; j--)
			{
				localRecords[j] = localRecords[j - 1];
			}
			localRecords[i] = r;

			fp = fopen(SAVEFILE, "wb");
			fwrite(localRecords, sizeof *localRecords, numRecords + 1, fp);
			fclose(fp);
			break;
		}	
	}
	fp = NULL;
}

void	initSession()
{
	SetMasterVolume(0.25);
	loadRecords(localRecords);
	for (int i = 0; i < ROCK_MAX; i++)
		rockPool[i].isLive = 0;
	gameState.rockCount = 0;
	gameState.rocksToSpawn = 4;
	gameState.numLives = 3;
	gameState.score = 0;
	gameState.oneUpMeter = 0;
	gameState.cleanTime = time - 2;
	gameState.currentScreen = GAME;
	gameState.isGameOver = false;
	player.angle = 0;
	player.deathTime = time - 2;
	saucer.isLive = false;
	saucer.bullets[0].isLive = false;
	saucer.bullets[1].isLive = false;
	saucer.mourningTimer = 11;
	saucer.shouldRespawn = false;
	saucer.requestTime = 0;
	saucer.inaccuracy = 10.0F;
	music.rest = 1.0;
}

void	endSession()
{
	SetMasterVolume(0);
	for (int i = 0; i < 10; i++)
	{
		if (gameState.score > localRecords[i].score)
		{
			gameState.currentScreen = HIGHSCORE;
			return;
		}
	}
	gameState.currentScreen = POSTGAME;
}

void	scoreAdd(unsigned short points)
{
	gameState.score += points;
	gameState.oneUpMeter += points;
	if (gameState.oneUpMeter > 10000)
	{
		gameState.numLives++;
		gameState.oneUpMeter -= 10000;
		PlaySound(sounds.extraShipSFX);
	}
}

void	drawLives()
{
	for (int i = 0; i < gameState.numLives; i++)
	{
		DrawTexturePro(
			player.texture,
			(Rectangle){0, 0, 21, 10},
			(Rectangle){86 + 10 * i, 42, player.currSprite.width, player.currSprite.height},
			player.CoM,
			-90,
			(Color){255, 255, 255, 127}
		);
	};
}

void	drawScore()
{
	static char	scoreStr[15];

	sprintf(scoreStr, "%8.2u", gameState.score);
	DrawTextEx(fontBold32, scoreStr, (Vector2){2, 0}, 32, 0, (Color){255, 255, 255, 127});
}

// PLAYER

void	decelerate()
{
	if (Vector2Length(player.vel) > 0)
		player.vel = Vector2Subtract(player.vel, Vector2Scale(player.vel, 0.25 * frameTime));
	player.currSprite.x = 0;
	sounds.thrustVolume -= 0.1F;
	if (sounds.thrustVolume < 0.0F)
		sounds.thrustVolume = 0.0F;
	SetSoundVolume(sounds.thrust, sounds.thrustVolume);
}

void	accelerate()
{
	float	speed;
	float	scale;
	
	player.vel.x += player.accel * cosf(player.angle * DEG2RAD) * frameTime;
	player.vel.y += player.accel * sinf(player.angle * DEG2RAD) * frameTime;
	speed = Vector2Length((Vector2){player.vel.x, player.vel.y});
	if (speed > player.maxSpeed)
	{
		scale = player.maxSpeed / speed;
		player.vel.x *= scale;
		player.vel.y *= scale;
	}
	player.currSprite.x = 21;
	
	sounds.thrustVolume += 0.1F;
	if (sounds.thrustVolume > 1.0F)
		sounds.thrustVolume = 1.0F;
	SetSoundVolume(sounds.thrust, sounds.thrustVolume);
	if (!IsSoundPlaying(sounds.thrust))
		PlaySound(sounds.thrust);
}

void	playerShoot()
{
	static unsigned	short	bulletIdx = 0;
	bool					isFull = true;
	float					bulletSpeed = player.maxSpeed;

	for (int i = 0; i < BULLET_MAX; i++)
		if (player.bulletPool[i].isLive == false)
			isFull = false;
	if (!isFull && !player.inHyperspace)
	{
		player.bulletPool[bulletIdx].pos = (Vector2){
			.x = player.pos.x + 8 * cosf(player.angle * DEG2RAD),
			.y = player.pos.y + 8 * sinf(player.angle * DEG2RAD)
		};
		player.bulletPool[bulletIdx].vel = (Vector2){
			.x = bulletSpeed * cosf(player.angle *DEG2RAD) + player.vel.x,
			.y = bulletSpeed * sinf(player.angle *DEG2RAD) + player.vel.y
		};
		player.bulletPool[bulletIdx].ttl = 1.3;
		player.bulletPool[bulletIdx].isLive = true;
		bulletIdx++;
		if (bulletIdx == BULLET_MAX)
			bulletIdx = 0;
		PlaySound(sounds.fireSFX);
	}
}

void	playerDie()
{
	explodeAt(player.hitBox.center);
	player.isLive = false;
	gameState.numLives--;
	player.deathTime = time;
	StopSound(sounds.thrust);
	PlaySound(sounds.bangMdSFX);
}

void	playerRespawn()
{
	for (int i = 0; i < ROCK_MAX; i++)
	{
		if (rockPool[i].isLive)
		{
			if (CheckCollisionCircleRec(
						rockPool[i].hitBox.center,
						rockPool[i].hitBox.radius,
						(Rectangle){SC_W/2 - 50, SC_H/2 - 50, 100, 100})
			) {
				return;
			}
		}
	}
	if (time - player.deathTime > 3)
	{
		if (saucer.isLive)
		{
			saucer.bullets[0].isLive = false;
			saucer.bullets[1].isLive = false;
			saucer.isLive = false;
			saucer.deathTime = time;
		}
		player.vel.x = 0;
		player.vel.y = 0;
		player.pos.x = SC_W / 2;
		player.pos.y = SC_H / 2;
		player.hitBox.center.x = SC_W / 2;
		player.hitBox.center.y = SC_H / 2;
		player.isLive = true;
		player.spawnTime = time;
	}
}

void	enterHyperspace()
{
	StopSound(sounds.thrust);
	player.hyperspaceTime = time;
	player.inHyperspace = true;
}

void	exitHyperspace()
{
	if (time - player.hyperspaceTime > 1.5)
	{
		player.vel.x = 0;
		player.vel.y = 0;
		player.pos.x = GetRandomValue(40, 600);
		player.pos.y = GetRandomValue(40, 440);
		player.hitBox.center.x = player.pos.x;
		player.hitBox.center.y = player.pos.y;
		player.inHyperspace = false;
		if (!GetRandomValue(0, 4))
			playerDie();
	}
}

void	updatePlayer()
{
	if (player.isLive)
	{
		player.pos.x += player.vel.x * frameTime;
		player.pos.y += player.vel.y * frameTime;
		player.hitBox.center.x += player.vel.x * frameTime;
		player.hitBox.center.y += player.vel.y * frameTime;
		if (player.pos.x > SC_W)
		{
			player.pos.x = 0;
			player.hitBox.center.x = 0;
		}
		if (player.pos.y > SC_H)
		{
			player.pos.y = 0;
			player.hitBox.center.y = 0;
		}
		if (player.pos.x < 0)
		{
			player.pos.x = SC_W;
			player.hitBox.center.x = SC_W;
		}
		if (player.pos.y < 0)
		{
			player.pos.y = SC_H;
			player.hitBox.center.y = SC_H;
		}
	}
}

void	handlePlayerCollisions()
{
	for (int i = 0; i < ROCK_MAX; i++)
	{
		if (rockPool[i].isLive && player.isLive)
			if (CheckCollisionCircles(player.hitBox.center, player.hitBox.radius, rockPool[i].hitBox.center, rockPool[i].hitBox.radius))
			{
				playerDie();
				decayRock(i, true);
			}
	}
	if (saucer.isLive)
		if (CheckCollisionCircles(player.hitBox.center, player.hitBox.radius, saucer.hitBox.center, saucer.hitBox.radius))
		{
			playerDie();
			saucerDie();
			if (saucer.size == BIG)
				scoreAdd(200);
			else
				scoreAdd(1000);
		}
	if (saucer.bullets[0].isLive)
		if (CheckCollisionPointCircle(saucer.bullets[0].pos, player.hitBox.center, player.hitBox.radius))
		{
			playerDie();
			saucer.bullets[0].isLive = false;
		}
	if (saucer.bullets[1].isLive)
		if (CheckCollisionPointCircle(saucer.bullets[1].pos, player.hitBox.center, player.hitBox.radius))
		{
			playerDie();
			saucer.bullets[1].isLive = false;
		}
}

void	handlePlayerBulletCollisions()
{
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (player.bulletPool[i].isLive)
		{
			for (int j = 0; j < ROCK_MAX; j++)
			{
				if (rockPool[j].isLive)
				{
					if (CheckCollisionPointCircle(player.bulletPool[i].pos, rockPool[j].hitBox.center, rockPool[j].hitBox.radius))
					{
						player.bulletPool[i].isLive = false;
						decayRock(j, true);
					}
				}
			}
			if (saucer.isLive)
			{
				if (CheckCollisionPointCircle(player.bulletPool[i].pos, saucer.hitBox.center, saucer.hitBox.radius))
				{
					player.bulletPool[i].isLive = false;
					saucerDie();
					if (saucer.size == BIG)
						scoreAdd(200);
					else
						scoreAdd(1000);
				}
			}
		}
	}
}

void	updatePlayerBullets()
{
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (player.bulletPool[i].isLive)
		{
			player.bulletPool[i].pos = Vector2Add(player.bulletPool[i].pos, Vector2Scale(player.bulletPool[i].vel, frameTime));
			if (player.bulletPool[i].pos.x > SC_W) player.bulletPool[i].pos.x = 0;
			if (player.bulletPool[i].pos.y > SC_H) player.bulletPool[i].pos.y = 0;
			if (player.bulletPool[i].pos.x < 0)    player.bulletPool[i].pos.x = SC_W;
			if (player.bulletPool[i].pos.y < 0)    player.bulletPool[i].pos.y = SC_H;
			player.bulletPool[i].ttl -= frameTime;
			if (player.bulletPool[i].ttl <= 0.0F)
				player.bulletPool[i].isLive = false;
		}
	}
}

void	drawPlayer()
{
	if (player.isLive && !player.inHyperspace)
	{
		DrawTexturePro(
			player.texture,
			player.currSprite,
			(Rectangle){player.pos.x, player.pos.y, player.currSprite.width, player.currSprite.height},
			player.CoM,
			player.angle,
			WHITE
		);
		//DrawCircleLinesV(player.hitBox.center, player.hitBox.radius, RED);
	}
}

void   drawPlayerBullets()
{
	for (int i = 0; i < BULLET_MAX; i++)
		if (player.bulletPool[i].isLive)
			DrawPixelV(player.bulletPool[i].pos, WHITE);
}

// SAUCER

static Vector2	getBestTarget()
{
	Vector2	best = player.pos;

	if (fabsf(saucer.pos.x - player.pos.x) > SC_W / 2)
		best.x += (player.pos.x < SC_W / 2) ? SC_W : -SC_W;
	if (fabsf(saucer.pos.y - player.pos.y) > SC_H / 2)
		best.y += (player.pos.y < SC_W / 2) ? SC_H : -SC_H;
	return (best);
}

void	saucerShoot()
{
	static bool	bulletIdx = 0;
	float		bulletSpeed = player.maxSpeed;
	float		angle;

	if (!saucer.bullets[bulletIdx].isLive)
	{
		switch (saucer.size)
		{
			case BIG:
				angle = GetRandomValue(0, 360) * DEG2RAD;
				break;
			case SMALL:
				angle = Vector2LineAngle(saucer.hitBox.center, getBestTarget()) + saucer.inaccuracy * DEG2RAD;
				break;
			default:
		}
		saucer.bullets[bulletIdx].pos = (Vector2){
			.x = saucer.hitBox.center.x + 5 * cosf(angle),
			.y = saucer.hitBox.center.y + 5 * sinf(angle)
		};
		saucer.bullets[bulletIdx].vel = (Vector2){
			.x = bulletSpeed * cosf(angle),
			.y = bulletSpeed * -sinf(angle)
		};
		saucer.bullets[bulletIdx].ttl = 1.3;
		saucer.bullets[bulletIdx].isLive = true;
		bulletIdx = !bulletIdx;
		saucer.shootTime = time;
		PlaySound(sounds.saucerFireSFX);
	}
	if (GetRandomValue(0, 1))
		saucer.inaccuracy *= -1;
}

void	saucerDie()
{
	explodeAt(saucer.hitBox.center);
	saucer.isLive = false;
	if (saucer.size == SMALL && saucer.inaccuracy != 0)
		saucer.inaccuracy += (saucer.inaccuracy < 0) ? 0.5F : -0.5F;
	saucer.deathTime = time;
	saucer.shouldRespawn = false;
	if (saucer.mourningTimer > 0)
		saucer.mourningTimer -= 1;
	switch (saucer.size)
	{
		case BIG:
			PlaySound(sounds.bangBgSFX);
			break;
		case SMALL:
			PlaySound(sounds.bangMdSFX);
			break;
		default:
			break;
	}
}

static void	spawnBgSaucer()
{
	saucer.size = BIG;
	saucer.hitBox.radius = 10;
	if (GetRandomValue(0, 1))
	{
		saucer.pos.x = -26;
		saucer.vel.x = saucer.speed;
	}
	else
	{
		saucer.pos.x = SC_W;
		saucer.vel.x = -saucer.speed;
	}
	saucer.pos.y = GetRandomValue(saucer.hitBox.radius * 2, SC_H - saucer.hitBox.radius * 2);
	saucer.vel.y = 0;
	saucer.hitBox.center.x = saucer.pos.x + 12.5;
	saucer.hitBox.center.y = saucer.pos.y + 8;
	saucer.spawnTime = time;
	saucer.isLive = true;
}

static void	spawnSmSaucer()
{
	saucer.size = SMALL;
	saucer.hitBox.radius = 5;
	if (GetRandomValue(0, 1))
	{
		saucer.pos.x = -14;
		saucer.vel.x = saucer.speed;
	}
	else
	{
		saucer.pos.x = SC_W;
		saucer.vel.x = -saucer.speed;
	}
	saucer.pos.y = GetRandomValue(saucer.hitBox.radius * 2, SC_H - saucer.hitBox.radius * 2);
	saucer.vel.y = 0;
	saucer.hitBox.center.x = saucer.pos.x + 6.5;
	saucer.hitBox.center.y = saucer.pos.y + 4.5;
	saucer.spawnTime = time;
	saucer.isLive = true;
}

void	requestSaucer()
{
	bool	isValid;

	if ((time - gameState.levelStartTime > 8)
			&& (!saucer.isLive)
			&& (time - saucer.deathTime > saucer.mourningTimer)
			&& ((gameState.rockCount <= 8 && gameState.rockCount > 0)
				|| gameState.currentScreen == POSTGAME))
	{
		isValid = true;
	}
	else
		isValid = false;
	if (isValid && !saucer.shouldRespawn)
	{
		saucer.shouldRespawn= true;
		saucer.requestTime = time;
	}
	if (!isValid)
		saucer.shouldRespawn = false;
	if ((saucer.shouldRespawn)
			&& (time - saucer.requestTime > 4)
			&& (time - player.spawnTime > 8))
	{
		if (gameState.score > 40000)
			spawnSmSaucer();
		else
		{
			if (time - gameState.levelStartTime > 60)
			{
				if (GetRandomValue(0, 9))
					spawnSmSaucer();
				else
					spawnBgSaucer();
			}
			else
			{
				if (gameState.score < 10000)
						spawnBgSaucer();
				else
				{
					if (GetRandomValue(0, 4))
						spawnBgSaucer();
					else
						spawnSmSaucer();
				}
			}
		}
		saucer.spawnTime = time;
		saucer.shouldRespawn = false;
	}
}

void	handleSaucerCollisions()
{
	for (int i = 0; i < ROCK_MAX; i++)
	{
		if (rockPool[i].isLive)
		{
			if (CheckCollisionCircles(saucer.hitBox.center, saucer.hitBox.radius, rockPool[i].hitBox.center, rockPool[i].hitBox.radius))
			{
				saucerDie();
				decayRock(i, false);
			}
		}
	}
}

void	handleSaucerBulletCollisions()
{
	if (saucer.bullets[0].isLive)
	{
		for (int i = 0; i < ROCK_MAX; i++)
		{
			if (rockPool[i].isLive)
			{
				if (CheckCollisionPointCircle(saucer.bullets[0].pos, rockPool[i].hitBox.center, rockPool[i].hitBox.radius))
				{
					saucer.bullets[0].isLive = false;
					decayRock(i, false);
				}
			}
		}
	}
	if (saucer.bullets[1].isLive)
	{
		for (int i = 0; i < ROCK_MAX; i++)
		{
			if (rockPool[i].isLive)
			{
				if (CheckCollisionPointCircle(saucer.bullets[1].pos, rockPool[i].hitBox.center, rockPool[i].hitBox.radius))
				{
					saucer.bullets[1].isLive = false;
					decayRock(i, false);
				}
			}
		}
	}
}

void	updateSaucer()
{
	static const short 				yComponent[3] = {-70, 0, 70};
	static struct {unsigned n: 2;}	idx = {};

	if (saucer.isLive)
	{
		saucer.pos = Vector2Add(saucer.pos, Vector2Scale(saucer.vel, frameTime));
		saucer.hitBox.center = Vector2Add(saucer.hitBox.center, Vector2Scale(saucer.vel, frameTime));
		if (time - saucer.spawnTime > 2)
		{
			if (idx.n == 0)
				saucer.vel.y = yComponent[GetRandomValue(1, 2)];
			else if (idx.n == 1)
				saucer.vel.y = yComponent[GetRandomValue(0, 2)];
			else
				saucer.vel.y = yComponent[GetRandomValue(0, 1)];
			idx.n++;
			saucer.spawnTime = time;
		}
		if (saucer.hitBox.center.y > SC_H)
		{
			saucer.hitBox.center.y = 0;
			saucer.pos.y = (saucer.size == BIG) ? -8 : -4;
		}
		if (saucer.hitBox.center.y < 0)
		{
			saucer.hitBox.center.y = SC_H;
			saucer.pos.y = (saucer.size == BIG) ? SC_H - 8 : SC_H - 4;
		}
		if (saucer.pos.x > SC_W + 1 || saucer.pos.x < -25)
		{
			saucer.isLive = false;
			saucer.deathTime = time;
			if (saucer.mourningTimer > 0)
				saucer.mourningTimer -= 1;
			if (saucer.size == SMALL && saucer.inaccuracy != 0)
				saucer.inaccuracy += (saucer.inaccuracy < 0) ? 0.5F : -0.5F;
		}
		if ((time - saucer.shootTime > 0.66)
				&& (saucer.hitBox.center.x < SC_W - saucer.hitBox.radius * 2)
				&& (saucer.hitBox.center.x > saucer.hitBox.radius * 2)
				&& (player.isLive || gameState.currentScreen == POSTGAME))
			saucerShoot();
		switch (saucer.size)
		{
			case BIG:
				if (!IsSoundPlaying(sounds.saucerBg))
					PlaySound(sounds.saucerBg);
				break;
			case SMALL:
				if (!IsSoundPlaying(sounds.saucerSm))
					PlaySound(sounds.saucerSm);
				break;
			default:
				break;
		}
	}
}

void	updateSaucerBullets()
{
	if (saucer.bullets[0].isLive)
	{
		saucer.bullets[0].pos = Vector2Add(saucer.bullets[0].pos, Vector2Scale(saucer.bullets[0].vel, frameTime));
		if (saucer.bullets[0].pos.x > SC_W) saucer.bullets[0].pos.x = 0;
		if (saucer.bullets[0].pos.y > SC_H) saucer.bullets[0].pos.y = 0;
		if (saucer.bullets[0].pos.x < 0)    saucer.bullets[0].pos.x = SC_W;
		if (saucer.bullets[0].pos.y < 0)    saucer.bullets[0].pos.y = SC_H;
		saucer.bullets[0].ttl -= frameTime;
		if (saucer.bullets[0].ttl <= 0.0F)
			saucer.bullets[0].isLive = false;
	}
	if (saucer.bullets[1].isLive)
	{
		saucer.bullets[1].pos = Vector2Add(saucer.bullets[1].pos, Vector2Scale(saucer.bullets[1].vel, frameTime));
		if (saucer.bullets[1].pos.x > SC_W) saucer.bullets[1].pos.x = 0;
		if (saucer.bullets[1].pos.y > SC_H) saucer.bullets[1].pos.y = 0;
		if (saucer.bullets[1].pos.x < 0)    saucer.bullets[1].pos.x = SC_W;
		if (saucer.bullets[1].pos.y < 0)    saucer.bullets[1].pos.y = SC_H;
		saucer.bullets[1].ttl -= frameTime;
		if (saucer.bullets[1].ttl <= 0.0F)
			saucer.bullets[1].isLive = false;
	}
}

void	drawSaucer()
{
	if (saucer.isLive)
	{
		if (saucer.size == BIG)
			DrawTexture(
				saucer.textureBg,
				saucer.pos.x,
				saucer.pos.y,
				WHITE
			);
		else 
			DrawTexture(
				saucer.textureSm,
				saucer.pos.x,
				saucer.pos.y,
				WHITE
			);
		//DrawCircleLinesV(saucer.hitBox.center, saucer.hitBox.radius, RED);
	}
}

void   drawSaucerBullets()
{
	if (saucer.bullets[0].isLive)
		DrawPixelV(saucer.bullets[0].pos, WHITE);
	if (saucer.bullets[1].isLive)
		DrawPixelV(saucer.bullets[1].pos, WHITE);
}

// ROCKS

void	spawnRocks()
{
	static struct {unsigned n: 2;}	idx = {};
	static const Rectangle			spawnAreas[4] = {
		{0, -10, SC_W, 10},
		{SC_W - 10, 0, SC_W + 10, SC_H},
		{0, SC_H - 10, SC_W, SC_H + 10},
		{-10, 0, 10, SC_H}
	};

	if ((time - gameState.cleanTime > 3)
			&& (!saucer.isLive)
			&& (time - saucer.deathTime > 2)
			&& (player.isLive || (gameState.currentScreen == POSTGAME))
			&& (time - player.spawnTime > 0.5))
	{
		for (int i = 0; i < gameState.rocksToSpawn * 4; i += 4)
		{
			rockPool[i] = (t_Rock){
				.hitBox.center.x = GetRandomValue(spawnAreas[idx.n].x, spawnAreas[idx.n].width),
				.hitBox.center.y = GetRandomValue(spawnAreas[idx.n].y, spawnAreas[idx.n].height),
				.hitBox.radius = 20,
				.textureIdx = GetRandomValue(0, 3),
				.size = BIG,
				.vel.x = GetRandomValue(35, 40),
				.isLive = true
			};
			rockPool[i].pos.x = rockPool[i].hitBox.center.x - 20;
			rockPool[i].pos.y = rockPool[i].hitBox.center.y - 20;
			rockPool[i].vel = Vector2Rotate(rockPool[i].vel, GetRandomValue(0, 360) * DEG2RAD);
			idx.n++;
			gameState.rockCount++;
		}
		if (gameState.rocksToSpawn + 2 < ROCK_SPAWN_MAX)
			gameState.rocksToSpawn += 2;
		else
			gameState.rocksToSpawn = ROCK_SPAWN_MAX;
		gameState.levelStartTime = time;
	}
}

void	decayRock(unsigned short rockIdx, bool	shouldScore)
{
	explodeAt(rockPool[rockIdx].hitBox.center);
	switch(rockPool[rockIdx].size)
	{
		case BIG:
			rockPool[rockIdx].size = MEDIUM;
			rockPool[rockIdx].pos.x += 10;
			rockPool[rockIdx].pos.y += 10;
			rockPool[rockIdx].hitBox.radius /= 2;
			rockPool[rockIdx + 2] = rockPool[rockIdx];
			rockPool[rockIdx].textureIdx = GetRandomValue(0, 3);
			rockPool[rockIdx + 2].textureIdx = GetRandomValue(0, 3);
			rockPool[rockIdx].vel = Vector2Scale(rockPool[rockIdx].vel, 0.25 * GetRandomValue(4, 8));
			rockPool[rockIdx].vel = Vector2Rotate(rockPool[rockIdx].vel, GetRandomValue(-90, 90) * DEG2RAD);
			rockPool[rockIdx + 2].vel = Vector2Scale(rockPool[rockIdx + 2].vel, 0.25 * GetRandomValue(4, 8));
			rockPool[rockIdx + 2].vel = Vector2Rotate(rockPool[rockIdx + 2].vel, GetRandomValue(-90, 90) * DEG2RAD);
			if (shouldScore)
				scoreAdd(20);
			gameState.rockCount++;
			PlaySound(sounds.bangBgSFX);
			break;
		case MEDIUM:
			rockPool[rockIdx].size = SMALL;
			rockPool[rockIdx].pos.x += 5;
			rockPool[rockIdx].pos.y += 5;
			rockPool[rockIdx].hitBox.radius /= 2;
			rockPool[rockIdx + 1] = rockPool[rockIdx];
			rockPool[rockIdx].textureIdx = GetRandomValue(0, 3);
			rockPool[rockIdx + 1].textureIdx = GetRandomValue(0, 3);
			rockPool[rockIdx].vel = Vector2Scale(rockPool[rockIdx].vel, 0.25 * GetRandomValue(4, 8));
			rockPool[rockIdx].vel = Vector2Rotate(rockPool[rockIdx].vel, GetRandomValue(-90, 90) * DEG2RAD);
			rockPool[rockIdx + 1].vel = Vector2Scale(rockPool[rockIdx + 1].vel, 0.25 * GetRandomValue(4, 8));
			rockPool[rockIdx + 1].vel = Vector2Rotate(rockPool[rockIdx + 1].vel, GetRandomValue(-90, 90) * DEG2RAD);
			if (shouldScore)
				scoreAdd(50);
			gameState.rockCount++;
			PlaySound(sounds.bangMdSFX);
			break;
		case SMALL:
			rockPool[rockIdx].isLive = false;
			if (shouldScore)
				scoreAdd(100);
			gameState.rockCount--;
			if (gameState.rockCount == 0)
			{
				gameState.cleanTime = time;
				music.rest = 1.0;
			}
			PlaySound(sounds.bangSmSFX);
			break;
	}
}

void	updateRocks()
{
	for (int i = 0; i < ROCK_MAX; i++)
	{
		rockPool[i].pos = Vector2Add(rockPool[i].pos, Vector2Scale(rockPool[i].vel, frameTime));
		rockPool[i].hitBox.center = Vector2Add(rockPool[i].hitBox.center, Vector2Scale(rockPool[i].vel, frameTime));
		if (rockPool[i].hitBox.center.x > SC_W)
		{
			rockPool[i].pos.x = 0 - rockPool[i].hitBox.radius;
			rockPool[i].hitBox.center.x = 0;
		}
		if (rockPool[i].hitBox.center.x < 0)
		{
			rockPool[i].pos.x = SC_W - rockPool[i].hitBox.radius;
			rockPool[i].hitBox.center.x = SC_W;
		}
		if (rockPool[i].hitBox.center.y > SC_H)
		{
			rockPool[i].pos.y = 0 - rockPool[i].hitBox.radius;
			rockPool[i].hitBox.center.y = 0;
		}
		if (rockPool[i].hitBox.center.y < 0)
		{
			rockPool[i].pos.y = SC_H - rockPool[i].hitBox.radius;
			rockPool[i].hitBox.center.y = SC_H;
		}
	}
}

void	drawRocks()
{
	for (int i = 0; i < ROCK_MAX; i++)
	{
		if (rockPool[i].isLive)
		{
			DrawTexture(
				rockTextures[rockPool[i].textureIdx + 4 * rockPool[i].size],
				rockPool[i].pos.x,
				rockPool[i].pos.y,
				WHITE
			);
			//char	stats[32] = { 0 };
			//DrawCircleLinesV(rockPool[i].hitBox.center, rockPool[i].hitBox.radius, RED);
			//sprintf(stats, "%.2f", Vector2Length(rockPool[i].vel)); 
			//DrawText(stats, rockPool[i].pos.x, rockPool[i].pos.y + 30, 10, YELLOW);
		}
	}
}

// RECORDS

unsigned short	loadRecords(t_Record *buffer)
{
	FILE			*fp;
	unsigned short	totalRead = 0;

	if ((fp = fopen(SAVEFILE, "rb")))
	{
		totalRead = fread(buffer, sizeof *localRecords, 10, fp);
		fclose(fp);
	}
	else {
		fp = fopen(SAVEFILE, "wb");
		fclose(fp);
	}
	fp = NULL;
	return (totalRead);
}

void	drawHighScore()
{
	char			str[10] = { 0 };
	unsigned short	scoreWidth = 0;
	unsigned int	highest = localRecords[0].score;

	if (highest == 0)
	{
		sprintf(str, "00");
		scoreWidth = 2;
	}
	else
	{
		while (highest > 0)
		{
			highest /= 10;
			scoreWidth++; 
		}
		sprintf(str, "%*u", scoreWidth, localRecords[0].score);
	}
	DrawTextEx(fontBold24, str, (Vector2){SC_W / 2 - 6 * scoreWidth, 0}, 24, 0, GRAY);
}
