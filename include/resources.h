#ifndef RESOURCES_H
# define RESOURCES_H

typedef struct	s_Sounds
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

void	playGameMusic();

void	loadAllResources();
void	unloadAllResources();

extern t_Sounds	sounds;
extern t_Music	music;
extern Font		font64;
extern Font		fontBold24;
extern Font		fontBold32;
extern Texture	rockTextures[12];

#endif
