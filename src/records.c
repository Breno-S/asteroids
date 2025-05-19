#include <stdio.h>

#include "main.h"
#include "records.h"
#include "resources.h"

t_Record	g_localRecords[10] = {0};	

unsigned short	loadRecords(t_Record *buffer)
{
	FILE			*fp;
	unsigned short	totalRead = 0;

	if ((fp = fopen(SAVEFILE, "rb")))
		totalRead = fread(buffer, sizeof *g_localRecords, 10, fp);
	else
		fp = fopen(SAVEFILE, "wb");
	fclose(fp);
	fp = NULL;
	return (totalRead);
}

void	saveRecord(char *initials, unsigned int score)
{
	FILE			*fp;
	t_Record		r = {{initials[0], initials[1], initials[2]}, score};
	unsigned short	numRecords;

	if (r.initials[0] == '_')
		r.initials[0] = ' ';
	if (r.initials[1] == '_')
		r.initials[1] = ' ';
	if (r.initials[2] == '_')
		r.initials[2] = ' ';
	numRecords = loadRecords(g_localRecords);
	for (int i = 0; i <= numRecords; i++)
	{
		if (g_localRecords[i].score < score)
		{
			for (int j = 9; j > i; j--)
				g_localRecords[j] = g_localRecords[j - 1];
			g_localRecords[i] = r;
			fp = fopen(SAVEFILE, "wb");
			fwrite(g_localRecords, sizeof *g_localRecords, numRecords + 1, fp);
			fclose(fp);
			break;
		}
	}
	fp = NULL;
}

void	drawHighScore(void)
{
	char			str[10] = {0};
	unsigned short	scoreWidth = 0;
	unsigned int	highest = g_localRecords[0].score;

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
		sprintf(str, "%*u", scoreWidth, g_localRecords[0].score);
	}
	DrawTextEx(g_fontBold24, str, (Vector2){SC_W / 2 - 6 * scoreWidth, 0}, 24, 0, GRAY);
}
