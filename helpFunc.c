/* ------------------------------------- */
/*               Yoav Levy               */
/*                                       */
/*            2023 Semester A            */
/* ------------------------------------- */

#include "utils.h"

/* This function copies string to other variable, untile a character: */
void copyUntilSign(char *line, char *to, char sign)
{
	int i = 0;

	if (to == NULL || line == NULL)
		return; /*NULL*/

	while (i < LINE_LENGTH && !isspace(line[i]) && line[i] != '\0' && line[i] != sign)
	{
		to[i] = line[i];
		i++;
	}

	to[i] = '\0';
}

/* This functio skips until a character: */
char *skipUntilSign(char *ch, char sign)
{
	if (ch == NULL)
		return NULL;
	while (!isspace(*ch) && (*ch) != sign)
		ch++;
	return ch;
}

/* This function checks if the we are at the end of a line. */
/* If so, returns TRUE, else returns FALSE. */
boolean endOfLine(char *line)
{
	return line == NULL || *line == '\0' || *line == '\n';
}

/* This function checks if we need to ignore a line: */
int ignoreLine(char *line)
{
	line = skipSpaces(line);
	return *line == ';' || *line == '\0' || *line == '\n';
}

/* This function skips every white space: */
char *skipSpaces(char *ch)
{
	if (ch == NULL)
		return NULL;
	while (isspace(*ch))
		ch++;
	return ch;
}

/* This function skips every character that is not awhite space: */
char *skipNotSpaces(char *ch)
{
	if (ch == NULL)
		return NULL;
	while (!(isspace(*ch)))
		ch++;
	return ch;
}

/* This function copies the next word on the line: */
void copyNextWord(char *line, char *to)
{
	int i = 0;

	if (to == NULL || line == NULL)
		return; /*NULL*/

	while (i < LINE_LENGTH && !isspace(line[i]) && line[i] != '\0')
	{
		to[i] = line[i];
		i++;
	}

	to[i] = '\0';
}

/* This function copies the next operand on the line: */
void copyNextOperandName(char *line, char *to)
{
	int i = 0;

	if (to == NULL || line == NULL)
		return; /*NULL*/

	while (i < LINE_LENGTH && !isspace(line[i]) && line[i] != ',' && line[i] != '\0')
	{
		to[i] = line[i];
		i++;
	}

	to[i] = '\0';
}
