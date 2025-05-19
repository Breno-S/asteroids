#ifndef RESOURCES_H
# define RESOURCES_H

typedef struct s_Sounds
{
	Sound	bangBgSFX;
	Sound	bangMdSFX;
	Sound	bangSmSFX;
	Sound	extraShipSFX;
	Sound	fireSFX;
	Sound	saucerFireSFX;
	Sound	saucerBg;
	Sound	saucerSm;
	Sound	thrust;
	float	thrustVolume;
}	t_Sounds;

typedef struct s_Music
{
	Sound	beat1;
	Sound	beat2;
	double	rest;
}	t_Music;

void	playGameMusic(void);

void	loadAllResources(void);
void	unloadAllResources(void);

extern t_Sounds	g_sounds;
extern t_Music	g_music;
extern Font		g_font64;
extern Font		g_fontBold24;
extern Font		g_fontBold32;
extern Texture	g_rockTextures[12];
extern Image	g_windowIcon;

#endif
