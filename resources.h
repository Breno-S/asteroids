#ifndef RESOURCES_H
# define RESOURCES_H

void	loadAllTextures();
void	loadAllFonts();

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

void	loadAllSounds();

typedef struct s_Music
{
	Sound	beat1;
	Sound	beat2;
	double	rest;
}	t_Music;

void	playGameMusic();

#endif
