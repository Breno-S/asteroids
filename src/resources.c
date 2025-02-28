#include "raylib.h"
#include "gameObjects.h"
#include "resources.h"

extern double		time;

t_Sounds	sounds;
t_Music		music = {.rest = 1.0};
Font		font64;
Font		fontBold32;
Font		fontBold24;
Texture		rockTextures[12];

static void	loadTextures()
{
	rockTextures[0]  = LoadTexture("assets/textures/Rock1-bg.png");
	rockTextures[1]  = LoadTexture("assets/textures/Rock2-bg.png");
	rockTextures[2]  = LoadTexture("assets/textures/Rock3-bg.png");
	rockTextures[3]  = LoadTexture("assets/textures/Rock4-bg.png");

	rockTextures[4]  = LoadTexture("assets/textures/Rock1-md.png");
	rockTextures[5]  = LoadTexture("assets/textures/Rock2-md.png");
	rockTextures[6]  = LoadTexture("assets/textures/Rock3-md.png");
	rockTextures[7]  = LoadTexture("assets/textures/Rock4-md.png");

	rockTextures[8]  = LoadTexture("assets/textures/Rock1-sm.png");
	rockTextures[9]  = LoadTexture("assets/textures/Rock2-sm.png");
	rockTextures[10] = LoadTexture("assets/textures/Rock3-sm.png");
	rockTextures[11] = LoadTexture("assets/textures/Rock4-sm.png");

	player.texture   = LoadTexture("assets/textures/Ship.png");
	saucer.textureBg = LoadTexture("assets/textures/Saucer-bg.png");
	saucer.textureSm = LoadTexture("assets/textures/Saucer-sm.png");
}

static void	unloadTextures()
{
	for (int i = 0; i < 12; i++)
		UnloadTexture(rockTextures[i]);
	UnloadTexture(player.texture);
	UnloadTexture(saucer.textureBg);
	UnloadTexture(saucer.textureSm);
}

static void	loadFonts()
{
	font64     = LoadFontEx("assets/fonts/font-hyperspace/Hyperspace.otf", 64, (void *)0, 0);
	fontBold32 =   LoadFont("assets/fonts/font-hyperspace/Hyperspace Bold.otf");
	fontBold24 = LoadFontEx("assets/fonts/font-hyperspace/Hyperspace Bold.otf", 24, (void *)0, 0);
}

static void	unloadFonts()
{
	UnloadFont(font64);
	UnloadFont(fontBold24);
	UnloadFont(fontBold32);
}

static void	loadSounds()
{
	sounds.bangBgSFX     = LoadSound("assets/sounds/bangLarge.wav");
	sounds.bangMdSFX     = LoadSound("assets/sounds/bangMedium.wav");
	sounds.bangSmSFX     = LoadSound("assets/sounds/bangSmall.wav");
	sounds.extraShipSFX  = LoadSound("assets/sounds/extraShip.wav");
	sounds.fireSFX       = LoadSound("assets/sounds/fire.wav");
	sounds.saucerFireSFX = LoadSound("assets/sounds/saucerFire.wav");
	sounds.saucerBg      = LoadSound("assets/sounds/saucerBig.wav");
	sounds.saucerSm      = LoadSound("assets/sounds/saucerSmall.wav");
	sounds.thrust        = LoadSound("assets/sounds/thrust.wav");
	music.beat1          = LoadSound("assets/sounds/beat1.wav");
	music.beat2          = LoadSound("assets/sounds/beat2.wav");
}

static void	unloadSounds()
{
	UnloadSound(sounds.bangBgSFX);
	UnloadSound(sounds.bangMdSFX);
	UnloadSound(sounds.bangSmSFX);
	UnloadSound(sounds.extraShipSFX);
	UnloadSound(sounds.fireSFX);
	UnloadSound(sounds.saucerFireSFX);
	UnloadSound(sounds.saucerBg);
	UnloadSound(sounds.saucerSm);
	UnloadSound(sounds.thrust);
	UnloadSound(music.beat1);
	UnloadSound(music.beat2);
}

void	loadAllResources()
{
	loadTextures();
	loadFonts();
	loadSounds();
}

void	unloadAllResources()
{
	unloadSounds();
	unloadFonts();
	unloadTextures();
}

void	playGameMusic()
{
	static bool		isDownBeat = true;
	static double	lastNoteTime = 0;
	static double	lastAccelerando = 0;
	Sound			note;

	if (isDownBeat)
		note = music.beat1;
	else
		note = music.beat2;
	if (time - lastNoteTime > music.rest
			&& gameState.rockCount > 0)
	{
		PlaySound(note);
		isDownBeat = !isDownBeat;
		lastNoteTime = time;
		if (time - lastAccelerando > 2 && player.isLive)
		{
			lastAccelerando = time;
			music.rest -= 0.05;
			if (music.rest < 0.25)
				music.rest = 0.25;
		}
	}
}
