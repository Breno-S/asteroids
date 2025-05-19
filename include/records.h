#ifndef RECORDS_H
# define RECORDS_H

typedef struct s_Record
{
	char			initials[3];
	unsigned int	score;
}	t_Record;

unsigned short	loadRecords(t_Record *buffer);
void			saveRecord(char *initials, unsigned int score);
void			drawHighScore(void);

extern t_Record	g_localRecords[10];

#endif
