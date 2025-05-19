#include "gameState.h"
#include "main.h"
#include "playerShip.h"
#include "raylib.h"
#include "resources.h"
#include "saucer.h"

t_Sounds	g_sounds;
t_Music		g_music = {.rest = 1.0};
Font		g_font64;
Font		g_fontBold32;
Font		g_fontBold24;
Texture		g_rockTextures[12];
Image		g_windowIcon;

/********************************** TEXTURES **********************************/

static void	loadTextures(void)
{
	g_rockTextures[0]  = LoadTexture("assets/textures/Rock1-bg.png");
	g_rockTextures[1]  = LoadTexture("assets/textures/Rock2-bg.png");
	g_rockTextures[2]  = LoadTexture("assets/textures/Rock3-bg.png");
	g_rockTextures[3]  = LoadTexture("assets/textures/Rock4-bg.png");

	g_rockTextures[4]  = LoadTexture("assets/textures/Rock1-md.png");
	g_rockTextures[5]  = LoadTexture("assets/textures/Rock2-md.png");
	g_rockTextures[6]  = LoadTexture("assets/textures/Rock3-md.png");
	g_rockTextures[7]  = LoadTexture("assets/textures/Rock4-md.png");

	g_rockTextures[8]  = LoadTexture("assets/textures/Rock1-sm.png");
	g_rockTextures[9]  = LoadTexture("assets/textures/Rock2-sm.png");
	g_rockTextures[10] = LoadTexture("assets/textures/Rock3-sm.png");
	g_rockTextures[11] = LoadTexture("assets/textures/Rock4-sm.png");

	g_player.texture   = LoadTexture("assets/textures/Ship.png");

	g_saucer.textureBg = LoadTexture("assets/textures/Saucer-bg.png");
	g_saucer.textureSm = LoadTexture("assets/textures/Saucer-sm.png");
}

static void	unloadTextures(void)
{
	for (int i = 0; i < 12; i++)
		UnloadTexture(g_rockTextures[i]);
	UnloadTexture(g_player.texture);
	UnloadTexture(g_saucer.textureBg);
	UnloadTexture(g_saucer.textureSm);
}

/*********************************** FONTS ************************************/

static void	loadFonts(void)
{
	g_font64     = LoadFontEx("assets/fonts/font-hyperspace/Hyperspace.otf", 64, (void *)0, 0);
	g_fontBold32 = LoadFont("assets/fonts/font-hyperspace/Hyperspace Bold.otf");
	g_fontBold24 = LoadFontEx("assets/fonts/font-hyperspace/Hyperspace Bold.otf", 24, (void *)0, 0);
}

static void	unloadFonts(void)
{
	UnloadFont(g_font64);
	UnloadFont(g_fontBold24);
	UnloadFont(g_fontBold32);
}

/*********************************** SOUNDS ***********************************/

static void	loadSounds(void)
{
	g_sounds.bangBgSFX     = LoadSound("assets/sounds/bangLarge.wav");
	g_sounds.bangMdSFX     = LoadSound("assets/sounds/bangMedium.wav");
	g_sounds.bangSmSFX     = LoadSound("assets/sounds/bangSmall.wav");
	g_sounds.extraShipSFX  = LoadSound("assets/sounds/extraShip.wav");
	g_sounds.fireSFX       = LoadSound("assets/sounds/fire.wav");
	g_sounds.saucerFireSFX = LoadSound("assets/sounds/saucerFire.wav");
	g_sounds.saucerBg      = LoadSound("assets/sounds/saucerBig.wav");
	g_sounds.saucerSm      = LoadSound("assets/sounds/saucerSmall.wav");
	g_sounds.thrust        = LoadSound("assets/sounds/thrust.wav");

	g_music.beat1          = LoadSound("assets/sounds/beat1.wav");
	g_music.beat2          = LoadSound("assets/sounds/beat2.wav");
}

static void	unloadSounds(void)
{
	UnloadSound(g_sounds.bangBgSFX);
	UnloadSound(g_sounds.bangMdSFX);
	UnloadSound(g_sounds.bangSmSFX);
	UnloadSound(g_sounds.extraShipSFX);
	UnloadSound(g_sounds.fireSFX);
	UnloadSound(g_sounds.saucerFireSFX);
	UnloadSound(g_sounds.saucerBg);
	UnloadSound(g_sounds.saucerSm);
	UnloadSound(g_sounds.thrust);

	UnloadSound(g_music.beat1);
	UnloadSound(g_music.beat2);
}

/******************************* ALL RESOURCES ********************************/

void	loadAllResources(void)
{
	loadTextures();
	loadFonts();
	loadSounds();
	g_windowIcon = LoadImage("assets/Icon.png");
}

void	unloadAllResources(void)
{
	unloadSounds();
	unloadFonts();
	unloadTextures();
	UnloadImage(g_windowIcon);
}

/********************************* PLAY MUSIC *********************************/

void	playGameMusic(void)
{
	static bool		isDownBeat = true;
	static double	lastNoteTime = 0;
	static double	lastAccelerando = 0;

	if ((g_time - lastNoteTime > g_music.rest) && (g_gameState.rockCount > 0))
	{
		if (isDownBeat)
			PlaySound(g_music.beat1);
		else
			PlaySound(g_music.beat2);
		isDownBeat = !isDownBeat;
		lastNoteTime = g_time;
		if (g_time - lastAccelerando > 2 && g_player.isLive)
		{
			lastAccelerando = g_time;
			g_music.rest -= 0.05;
			if (g_music.rest < 0.25)
				g_music.rest = 0.25;
		}
	}
}
