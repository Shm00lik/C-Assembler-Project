/* ------------------------------------- */
/*               Yoav Levy               */
/*                                       */
/*            2023 Semester A            */
/* ------------------------------------- */

#include "preProcessor.h"
#define LENGTH_OF_WORD_MCR 4
#define LENGTH_OF_WORD_ENDMCR 7

/*program spreads macros in the file given*/
/*returns 1 in case of an error (file wont open or allocation wasnt successful), else return 0*/
int preAssembler(char filename[])
{
	FILE *fd, *fd2;
	char filename_new[FILENAME_MAX];

	strcpy(filename_new, filename); /*get the name of the file to read from*/
	strcat(filename_new, ".as");
	fd = fopen(filename_new, "r"); /* open the file that we should spread the macro in */

	if (fd == NULL) /* file didnt open correctly */
	{
		printf("Can't open %s!\n", filename_new);
		return FAIL;
	}

	/*check if file is empty*/
	fseek(fd, 0, SEEK_END);
	if (ftell(fd) == 0) /* if the file is empty */
	{
		printf("%s is empty!\n", filename_new);
		fclose(fd);
		return FAIL;
	}
	rewind(fd);

	strcpy(filename_new, filename); /*get the name of the file to write to*/
	strcat(filename_new, ".am");
	fd2 = fopen(filename_new, "w"); /* open file for the code after macro was spread */

	if (spreadMacros(fd, fd2))
	{
		printf("Error! not enough space for memory allocation!");
		fclose(fd);
		remove(filename_new);
		return FAIL;
	}
	fclose(fd);
	fclose(fd2);
	return SUCCESS;
}

/*************************************/

/*************************************/

/*main function for spreading macros*/
/*return 0 if successful, else 1*/
int spreadMacros(FILE *fd, FILE *dest)
{
	char line[LINE_LENGTH];		  /* save lines (sentences) here */
	char macro_name[LINE_LENGTH]; /* save macro name here*/

	int kind; /*1 for mcr, 2 for endmcr, else 0*/

	macro *head = NULL;
	macro *curr=NULL, *temp=NULL;
	boolean is_set = FALSE;

	int is_first_line;

	boolean ismcr = FALSE; /* 1 if program is inside macro definition else 0 */
	

	while (fgets(line, LINE_LENGTH, fd) != NULL)
	{
		kind = whichKind(line, macro_name); /*1 if the line is starting a macro (mcr), 2 if its ending a macro (endmcr), else 0*/
	
		if (kind == ENDMCR) /* macro definition has ended */
		{
			ismcr = FALSE;
			temp = curr;
		}
		else if (kind == MCR && nameValid(macro_name)) /* macro definition starts */
		{
			ismcr = TRUE;
			is_first_line = TRUE;

			curr = (macro *)malloc(sizeof(macro));
			if (curr == NULL)
			{
				freeList(head);
				return FAIL;
			}
			if (is_set)
			{
				temp->next = curr;
			}
			curr->next = NULL;

			curr->name = (char *)malloc(sizeof(char) * (strlen(macro_name) + 1));
			if (curr->name == NULL)
			{
				freeList(head);
				return FAIL;
			}
			strcpy(curr->name, macro_name);

			if (!is_set)
			{
				head = curr;
				is_set = TRUE;
			}
		}
		else /* not starting macro definition and not ending one*/
		{
			if (ismcr == FALSE)
			{

				/* write line to the new file*/
				int is_call = is_valid2(line, macro_name);
				int index = which_mcr(macro_name, head, dest);
				if (is_call && index)
				{
					int i = 1;
					macro *tmp = head;
					while (i < index)
					{
						tmp = tmp->next;
						i++;
					}
					fprintf(dest, "%s", tmp->lines);
				}
				else
				{
					fprintf(dest, "%s", line);
				}
			}
			else
			{
				if (is_first_line)
				{
					curr->lines = (char *)malloc(sizeof(char) * LINE_LENGTH);
				}
				else
				{
					curr->lines = (char *)realloc(curr->lines, sizeof(char) * (LINE_LENGTH + strlen(curr->lines)));
				}
				if (curr->lines == NULL)
				{
					freeList(head);
					return FAIL;
				}
				if (is_first_line) strcpy(curr->lines, line);
				else strcat(curr->lines, line);
				is_first_line = FALSE;
			}
		}
	}
	
	freeList(head); /* free dynamic allocations */
	return SUCCESS;
}

/**/
int which_mcr(char word[], macro *head, FILE *dest)
{
	int i = 1;
	while (head != NULL)
	{
		if (strcmp(word, head->name) == 0)
			return i;

		head = head->next;
		i++;
	}
	return 0;
}

/**/
int is_valid2(char line[], char word[])
{
	int i = 0, z = 0;
	int error = 0;

	while (line[i] == ' ' || line[i] == '\t')
		i++;
	if (line[i] == '\n')
		error = 1;
	if (!error)
	{
		while (line[i] != '\t' && line[i] != ' ' && line[i] != '\n')
		{
			word[z] = line[i];
			z++;
			i++;
		}
		word[z] = '\0';

		while (line[i] == ' ' || line[i] == '\t')
			i++;
		if (line[i] == '\n')
			return 1;
	}

	return 0;
}

/*****************/

/*free list*/
int freeList(macro *head)
{
	macro *temp;
	while (head != NULL)
	{
		temp = head;

		free(head->lines);
		free(head->name);
		free(head);
		head = temp->next;
	}
	return 0;
}

/*check if macro name is valid. return 1 if it is valid else return 0*/
int nameValid(char name[])
{
	if(isCommand(name) != 0) return 0;
	if (isRegister(name))
		return 0;
	return 1;
}

/* return 1 if the command is starting a macro (mcr), return 2 if its ending a macro (endmcr), else return 0*/
/* if the line has macro defenition the macro name is written into "name"*/
int whichKind(char line[], char name[])
{
	/*indexes*/
	int i = 0;
	int j = 0;
	int z = 0;

	char word1[LENGTH_OF_WORD_MCR];
	char word2[LENGTH_OF_WORD_ENDMCR];
	int error = 0;

	while (line[i] == ' ' || line[i] == '\t')
		i++;

	/*check if command is starting a macro*/
	while ((line[i + j] != '\n') && (j < 3))
	{
		word1[j] = line[i + j];
		j++;
	}
	word1[j] = '\0';

	if (strcmp(word1, "mcr") == 0)
	{
		if (line[i + j] != '\t' && line[i + j] != ' ')
			error = 1;
		if (!error)
		{
			while (line[i + j] == ' ' || line[i + j] == '\t')
				j++;
			if (line[i + j] == '\n')
				error = 1;
			if (!error)
			{
				while (line[i + j] != '\t' && line[i + j] != ' ' && line[i + j] != '\n')
				{
					name[z] = line[i + j];
					z++;
					j++;
				}
				name[z] = '\0';

				while (line[i + j] == ' ' || line[i + j] == '\t')
					j++;
				if (line[i + j] == '\n')
					return MCR;
			}
		}
	}

	/* check if command is ending a macro*/
	j = 0;
	while (line[i + j] != '\n' && j < (6))
	{
		word2[j] = line[i + j];
		j++;
	}
	word2[j] = '\0';

	if (strcmp(word2, "endmcr") == 0)
	{
		while (line[i + j] != '\n')
		{
			if (line[i + j] != '\t' && line[i + j] != ' ')
				error = 1;
			j++;
		}
		if (!error)
			return ENDMCR;
	}

	/*command is not starting a macro nor ending a macro*/
	return 0;
}
