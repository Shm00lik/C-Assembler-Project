/* ------------------------------------- */
/*               Yoav Levy               */
/*                                       */
/*            2023 Semester A            */
/* ------------------------------------- */

#include "firstPass.h"

/* Some constants: */
const char *firstGroupCommands[NUM_OF_COMMANDS_FIRST_GROUP] = {"mov", "cmp", "add", "sub", "lea"};
const char *secondGroupCommands[NUM_OF_COMMANDS_SECOND_GROUP] = {"not", "clr", "inc", "dec", "jmp", "bne", "red", "prn", "jsr"};
const char *thirdGroupCommands[NUM_OF_COMMANDS_THIRD_GROUP] = {"rts", "stop"};
const char *jumpWithParameters[NUM_OF_JWP] = {"jmp", "bne", "jsr"};

const char *dataDirectives[NUM_OF_DATA_DIRECTIVES] = {".data", ".string"};
const char *labelDirectives[NUM_OF_LABEL_DIRECTIVES] = {".entry", ".extern"};
const char *directiveNames[NUM_OF_DATA_DIRECTIVES + NUM_OF_LABEL_DIRECTIVES] = {"entry", "extern", "data", "string"};

/* Setting up variables that we will use through the first pass: */
int error;
int lineError;
int ic;
int dc;
int lineNum;
commandName whichCommand; /* which opcode the command has (technicly opcode + 1)*/

adressingSystem targetOpAdressing; /* which adressing system is the target operand */
adressingSystem sourceOpAdressing; /* which adressing system is the source operand */
adressingSystem param1Adressing;   /* which adressing system is parameter number 1 */
adressingSystem param2Adressing;   /* which adressing system is parameter number 2 */

int reg1; /*which register number*/
int reg2; /*which register number*/

/* This function makes the first pass. */
isFail firstPass(FILE *fp, int codeArr[], int dataArr[], label **labelsList, labelOperands **labelOperandsList, entryLabel **entryList, int length[])
{
	/* Resetting the variables */
	char line[LINE_LENGTH];
	error = SUCCESS;
	lineNum = 1;
	ic = MEMORY_LOAD_START;
	dc = 0;

	/* Going through every line of the file: */
	while (fgets(line, LINE_LENGTH, fp) != NULL)
	{
		lineError = SUCCESS;

		/* Ignore this line, if needed: */
		if (!ignoreLine(line))
		{
			/* Process the line */
			processLine(line, codeArr, dataArr, labelsList, labelOperandsList, entryList);
		}

		lineNum += 1;

		if (lineError != SUCCESS)
		{
			error = lineError;
		}
	}

	/* Offset all of the labels that marked as data, adding ic to dc: */
	offsetDataLabels(*labelsList, ic);

	/* Join the code and data arrays: */
	joinDataAndCommand(codeArr, dataArr);

	/* Set the length of the code and data arrays: */
	length[0] = ic - MEMORY_LOAD_START;
	length[1] = dc;

	return error;
}

/* This function processes a line: */
void processLine(char *line, int codeArr[], int dataArr[], label **labelsList, labelOperands **labelOperandsList, entryLabel **entryList)
{
	/* Setting up variables: */
	boolean hasLabel = FALSE;
	char label[LABEL_LENGTH];
	char currentWord[LINE_LENGTH];

	/* Skip spaces: */
	line = skipSpaces(line);

	/* If the line is empty, ignore it: */
	if (endOfLine(line))
	{
		return;
	}

	/* Copy the first word of the line: */
	copyNextWord(line, currentWord);

	/* Checking if there is : in the current word: */
	if (strstr(currentWord, ":") != NULL)
	{
		/* If so, saving only the label to the current word: */
		copyUntilSign(currentWord, currentWord, ':');

		/* If the first word is a label, handle it: */
		if (isLabelDefinition(currentWord))
		{
			/* Set the label: */
			hasLabel = TRUE;
			strcpy(label, currentWord);

			/* Skip the label and spaces, and copy the next word: */
			line = skipSpaces(skipUntilSign(line, ':') + 1);
			copyNextWord(line, currentWord);

			/* If the next word is a macro, print an error: */
			if (strcmp(currentWord, "mcr") == 0)
			{
				printf("ERROR: Macro can't be define after a label (LINE %d).\n", lineNum);
				lineError = FAIL;
				return;
			}

			/* If the line is empty, there is an error: */
			if (endOfLine(line))
			{
				printf("ERROR: Missing command after label (LINE %d).\n", lineNum);
				lineError = FAIL;
				return;
			}
		}
	}

	/* If the first word is a data directive, handle it: */
	if (isDataDirective(currentWord))
	{
		/* If there is a label, add it to the list and mark as data: */
		if (hasLabel)
		{
			if (!addLabel(labelsList, label, dc, FALSE, FALSE, TRUE))
			{
				/* If the label already exists, print an error: */
				printf("ERROR: Label %s already exists (LINE %d).\n", label, lineNum);
			}
		}

		/* Handle the data directive: */
		handleDataDirective(line, currentWord, dataArr);

		return;
	}

	/* If the first word is a label directive, handle it: */
	if (isLabelDirective(currentWord))
	{
		/* If there is a label, print a warning: */
		if (hasLabel)
		{
			printf("WARNING: Declaration of label in .entry or .extern command (LINE %d).\n", lineNum);
		}

		/* Handle the label directive: */
		handleLabelDirective(line, currentWord, labelsList, entryList);

		return;
	}

	/* Add the label to the list */
	if (hasLabel)
	{
		if (!addLabel(labelsList, label, ic, FALSE, FALSE, FALSE))
		{
			/* If the label already exists, print an error: */
			printf("ERROR: Label %s already exists (LINE %d).\n", label, lineNum);
		}
	}

	/* Getting which command is the next word: */
	whichCommand = isCommand(currentWord);

	/* Setting the registers to be NO_REG */
	reg1 = NO_REG;
	reg2 = NO_REG;

	/* If the next word is a command, handle it: */
	if (whichCommand && lineError == SUCCESS)
	{
		/* Saving the number of arguments that the command needs: */
		int numOfArgs;
		numOfArgs = whichCommandGroup(currentWord);

		if (numOfArgs == TwoArg)
		{
			/* command with two arguments: */
			if (!twoArgAdressing(line, currentWord, codeArr, labelOperandsList))
				return;
		}

		else if (numOfArgs == OneArg)
		{
			/* command with one argument: */
			sourceOpAdressing = NoOperand;

			/* If the command is a jump command, handle it "differently": */
			if (isJumpCommand(currentWord))
			{
				if (!JumpAdressing(line, currentWord, codeArr, labelOperandsList))
					return;
			}
			else
			{
				/* If the command is not a jump command, handle it "normally": */
				if (!oneArgAdressing(line, currentWord, codeArr, labelOperandsList))
					return;
			}
		}
		else if (numOfArgs == ZeroArg)
		{
			/* command with no arguments: */
			if (!zeroArgAdressing(line, currentWord))
				return;
		}

		/* If there is no error, add the command to the code array: */
		if (lineError == SUCCESS)
			codeFirstWord(codeArr, whichCommand, ic, targetOpAdressing, sourceOpAdressing, param1Adressing, param2Adressing);

		/* Update the IC: */
		updateIc();
	}

	/* If the command is invalid: */
	else if (lineError == SUCCESS)
	{
		printf("ERROR: Invalid command name (LINE %d).\n", lineNum);
		lineError = FAIL;
		return;
	}
}

/* This function handles the label directives ('.extern' and '.entry') */
void handleLabelDirective(char *line, char *currentWord, label **labelsList, entryLabel **entryList)
{
	boolean isExtern = FALSE;

	/* If the first word is '.extern': */
	if (strcmp(currentWord, ".extern") == 0)
	{
		isExtern = TRUE;
	}

	/* Skip spaces and copy the next word: */
	line = skipSpaces(skipNotSpaces(line));
	copyNextWord(line, currentWord);

	/* If the next word is empty, there is an error: */
	if (endOfLine(line))
	{
		printf("ERROR: Missing label after .extern or .entry (LINE %d).\n", lineNum);
		lineError = FAIL;
		return;
	}

	/* If the next word is not a valid label, print an error: */
	if (!isValidLabel(currentWord))
	{
		printf("ERROR: Invalid label name (LINE %d).\n", lineNum);
		lineError = FAIL;
		return;
	}

	/* If the next word is a valid label, add it to the list: */
	if (!isExtern)
	{
		/* Add it to the entry labels list: */
		addLabelEntry(entryList, currentWord, lineNum);
		return;
	}

	if (!addLabel(labelsList, currentWord, 0, TRUE, FALSE, FALSE))
	{
		/* If the label already exists, print an error: */
		printf("ERROR: Label %s already exists (LINE %d).\n", currentWord, lineNum);
		lineError = FAIL;
		return;
	}
}

/* This function handles the data directives ('.data' and '.string') */
void handleDataDirective(char *line, char *currentWord, int dataArr[])
{
	char *pt;
	char *lineCopy;
	char isLastCharComma = FALSE;
	int num;

	/* If the first word is '.string': */
	if (strcmp(currentWord, ".string") == 0)
	{
		/* Skip spaces and copy the next word: */
		line = skipSpaces(skipNotSpaces(line));

		/* If the next word is empty, there is an error: */
		if (endOfLine(line))
		{
			printf("ERROR: Missing string after .string (LINE %d).\n", lineNum);
			lineError = FAIL;
			return;
		}

		/* If the next word does not start with ", print an error: */
		if (line[0] != '"')
		{
			printf("ERROR: Missing \" at the start of .string decleration (LINE %d).\n", lineNum);
			lineError = FAIL;
			return;
		}

		/* Skipping the " */
		line = line + 1;

		/* Copying the line to a new string: */
		lineCopy = (char *)malloc(strlen(line) + 1);
		strcpy(lineCopy, line);

		pt = lineCopy;

		/* If the next word does not end with ", print an error: */
		while (pt[0] != '"')
		{
			if (endOfLine(pt))
			{
				printf("ERROR: Missing \" at the end of .string decleration (LINE %d).\n", lineNum);
				lineError = FAIL;
				free(lineCopy);

				return;
			}

			pt = pt + 1;
		}

		pt = pt + 1;

		/* If there are extra characters after the string, print an error: */
		if (!endOfLine(skipSpaces(pt)))
		{
			printf("ERROR: Extra characters after .string decleration (LINE %d).\n", lineNum);
			lineError = FAIL;
			free(lineCopy);
			return;
		}

		free(lineCopy);

		/* Copying the string to the data array: */
		while (line[0] != '"')
		{
			if (endOfLine(line))
			{
				printf("ERROR: Missing \" at the end of .string decleration (LINE %d).\n", lineNum);
				lineError = FAIL;
				return;
			}

			dataArr[dc] = line[0];
			dc++;
			line = line + 1;
		}

		dataArr[dc] = '\0';
		dc++;
	}
	else
	{
		/* If the first word is '.data': */

		/* Skip spaces and copy the next word: */
		line = skipSpaces(skipNotSpaces(line));

		/* If the next word is empty, there is an error: */
		if (endOfLine(line))
		{
			printf("ERROR: Missing number after .data (LINE %d).\n", lineNum);
			lineError = FAIL;
			return;
		}

		/* Copying the line to a new string: */
		lineCopy = (char *)malloc(strlen(line) + 1);
		strcpy(lineCopy, line);

		pt = lineCopy;

		/* Validating the numbers after '.data': */
		while (!endOfLine(pt))
		{
			if (pt[0] == ',')
			{
				if (isLastCharComma)
				{
					printf("ERROR: Missing number after , (LINE %d).\n", lineNum);
					lineError = FAIL;
					free(lineCopy);
					return;
				}

				isLastCharComma = TRUE;
				pt = skipSpaces(pt + 1);
				continue;
			}

			if (!isdigit(pt[0]) && pt[0] != '-' && pt[0] != '+')
			{
				printf("ERROR: Invalid number in .data decleration (LINE %d).\n", lineNum);
				lineError = FAIL;
				free(lineCopy);
				return;
			}

			isLastCharComma = FALSE;
			pt = skipSpaces(pt + 1);
		}

		/* Splitting by ','*/
		pt = strtok(line, ",");

		while (pt != NULL)
		{
			/* Saving every number to the data array: */
			num = atoi(pt);
			dataArr[dc] = num;
			dc++;

			pt = strtok(NULL, ",");
		}
	}
}

/* This function handles commands with two arguments. */
/* Return FALSE if theres an error in the command, else - save which adressing systems command is using. */
boolean twoArgAdressing(char *line, char *currentWord, int codeArr[], labelOperands **labelOperandsList)
{
	char *pt;
	pt = skipNotSpaces(line);
	pt = skipSpaces(pt);
	if (endOfLine(pt))
	{
		printf("ERROR: Missing source operand (LINE %d).\n", lineNum);
		lineError = FAIL;
		return FALSE;
	}
	copyNextOperandName(pt, currentWord);
	if (isImmediate(currentWord))
	{
		if (!isValidAdressingSystem(Immediate, whichCommand, 1))
			return FALSE;
		sourceOpAdressing = Immediate;
		/*******/
		codeImmediate(codeArr, ic, immediateValue(currentWord));
	}
	else if (isRegister(currentWord))
	{
		if (!isValidAdressingSystem(Register, whichCommand, 1))
			return FALSE;
		sourceOpAdressing = Register;
		reg1 = currentWord[1] - '0';
	}
	else
	{
		if (!isValidLabel(currentWord))
		{
			printf("ERROR: Illegal operand (LINE %d).\n", lineNum);
			lineError = FAIL;
			return FALSE;
		}
		sourceOpAdressing = Label;
		addLabelOperands(labelOperandsList, currentWord, ic + 1, lineNum);
	}

	pt = skipComma(pt);
	if (lineError == FAIL)
		return FALSE;
	if (endOfLine(pt))
	{
		printf("ERROR: Missing target operand (LINE %d).\n", lineNum);
		lineError = FAIL;
		return FALSE;
	}
	if ((*pt) == ',')
	{
		printf("ERROR: Multiple consecutive comma (LINE %d).\n", lineNum);
		lineError = FAIL;
		return FALSE;
	}

	copyNextOperandName(pt, currentWord);

	if (isImmediate(currentWord))
	{

		if (!isValidAdressingSystem(Immediate, whichCommand, 0))
			return FALSE;
		targetOpAdressing = Immediate;
		codeImmediate(codeArr, ic + 2, immediateValue(currentWord));
	}
	else if (isRegister(currentWord))
	{
		targetOpAdressing = Register;
		reg2 = currentWord[1] - '0';
	}
	else
	{
		if (!isValidLabel(currentWord))
		{
			printf("ERROR: Illegal operand (LINE %d).\n", lineNum);
			lineError = FAIL;
			return FALSE;
		}
		targetOpAdressing = Label;
		addLabelOperands(labelOperandsList, currentWord, ic + 2, lineNum);
	}
	pt = skipNotSpaces(pt);
	pt = skipSpaces(pt);
	if (!endOfLine(pt))
	{
		printf("ERROR: Extraneous text after end of command (LINE %d).\n", lineNum);
		lineError = FAIL;
		return FALSE;
	}

	codeRegister(codeArr, ic, reg1, reg2);

	return TRUE;
}

/* This function handles commands with one argument. */
/* Return FALSE if theres an error in the command, else - save which adressing systems command is using. */
boolean oneArgAdressing(char *line, char *currentWord, int codeArr[], labelOperands **labelOperandsList)
{
	char *pt;
	pt = skipNotSpaces(line);
	pt = skipSpaces(pt);

	if (endOfLine(pt))
	{
		printf("ERROR: Missing operand (LINE %d).\n", lineNum);
		lineError = FAIL;
		return FALSE;
	}

	copyNextWord(pt, currentWord);
	if (isImmediate(currentWord))
	{
		if (!isValidAdressingSystem(Immediate, whichCommand, 0))
			return FALSE;
		targetOpAdressing = Immediate;

		/*******/
		codeImmediate(codeArr, ic, immediateValue(currentWord));
	}
	else if (isRegister(currentWord))
	{
		targetOpAdressing = Register; /**/
		reg1 = currentWord[1] - '0';
	}
	else
	{
		if (!isValidLabel(currentWord))
		{
			printf("ERROR: Illegal operand (LINE %d).\n", lineNum);
			lineError = FAIL;
			return FALSE;
		}
		targetOpAdressing = Label; /**/
		addLabelOperands(labelOperandsList, currentWord, ic + 1, lineNum);
	}

	pt = skipNotSpaces(pt);
	pt = skipSpaces(pt);
	if (!endOfLine(pt))
	{
		printf("ERROR: Extraneous text after end of command (LINE %d).\n", lineNum);
		lineError = FAIL;
		return FALSE;
	}

	codeRegister(codeArr, ic, reg1, reg2);
	return TRUE;
}

/* This function handles commands with no arguments. */
/* Return FALSE if theres an error in the command, else - save which adressing systems command is using. */
boolean zeroArgAdressing(char *line, char *currentWord)
{
	char *pt;
	pt = skipNotSpaces(line);
	pt = skipSpaces(pt);
	if (!endOfLine(pt))
	{
		printf("ERROR: Extraneous text after end of command (LINE %d).\n", lineNum);
		lineError = FAIL;
		return FALSE;
	}

	targetOpAdressing = NoOperand;
	sourceOpAdressing = NoOperand;
	return TRUE;
}

/* This function handles jump commands. */
/* Return FALSE if theres an error in the command, else - save which adressing systems command is using. */
boolean JumpAdressing(char *line, char *currentWord, int codeArr[], labelOperands **labelOperandsList)
{
	char *pt;
	pt = skipNotSpaces(line);
	pt = skipSpaces(pt);
	if (endOfLine(pt))
	{
		printf("ERROR: Missing operand (LINE %d).\n", lineNum);
		lineError = FAIL;
		return FALSE;
	}
	copyUntilSign(pt, currentWord, '(');
	if (!isValidLabel(currentWord))
	{
		printf("ERROR: Illegal operand (LINE %d).\n", lineNum);
		lineError = FAIL;
		return FALSE;
	}
	pt = skipUntilSign(pt, '(');

	if (isspace(*pt))
	{
		pt = skipSpaces(pt);
		if (endOfLine(pt))
		{
			targetOpAdressing = Label;
			addLabelOperands(labelOperandsList, currentWord, ic + 1, lineNum);
		}
		else
		{
			printf("ERROR: Extraneous text after end of command (LINE %d).\n", lineNum);
			lineError = FAIL;
			return FALSE;
		}
	}
	else
	{
		pt++;
		addLabelOperands(labelOperandsList, currentWord, ic + 1, lineNum);
		if (isspace(*pt))
		{
			pt = skipSpaces(pt);
			if ((*pt) == ',')
			{
				printf("ERROR: Missing parameter (LINE %d).\n", lineNum);
				lineError = FAIL;
				return FALSE;
			}
			else
			{
				printf("ERROR: Structure of command incorrect (LINE %d).\n", lineNum);
				lineError = FAIL;
				return FALSE;
			}
		}
		copyUntilSign(pt, currentWord, ',');

		/**/
		if (isImmediate(currentWord))
		{
			param1Adressing = Immediate;
			/*******/
			codeImmediate(codeArr, ic + 1, immediateValue(currentWord));
		}
		else if (isRegister(currentWord))
		{
			param1Adressing = Register;
			reg1 = currentWord[1] - '0';
		}
		else
		{
			if (!isValidLabel(currentWord))
			{
				printf("ERROR: Illegal parameter (LINE %d).\n", lineNum);
				lineError = FAIL;
				return FALSE;
			}
			param1Adressing = Label;
			addLabelOperands(labelOperandsList, currentWord, ic + 2, lineNum);
		}
		/**/

		pt = skipUntilSign(pt, ',');
		if (isspace(*pt))
		{
			pt = skipSpaces(pt);
			if (endOfLine(pt))
			{
				printf("ERROR: Structure of command incorrect (LINE %d).\n", lineNum);
				lineError = FAIL;
				return FALSE;
			}
			else if (*pt == ',')
			{
				printf("ERROR: Structure of command incorrect (LINE %d).\n", lineNum);
				lineError = FAIL;
				return FALSE;
			}
			else
			{
				printf("ERROR: Comma missing (LINE %d).\n", lineNum);
				lineError = FAIL;
				return FALSE;
			}
		}
		else
		{
			pt++;
			if (isspace(*pt))
			{
				pt = skipSpaces(pt);
				if ((*pt) == ')')
				{
					printf("ERROR: Missing parameter (LINE %d).\n", lineNum);
					lineError = FAIL;
					return FALSE;
				}
				else
				{
					printf("ERROR: Structure of command incorrect (LINE %d).\n", lineNum);
					lineError = FAIL;
					return FALSE;
				}
			}

			copyUntilSign(pt, currentWord, ')');

			/**/
			if (isImmediate(currentWord))
			{
				param2Adressing = Immediate;
				/*******/
				codeImmediate(codeArr, ic + 2, immediateValue(currentWord));
			}
			else if (isRegister(currentWord))
			{
				param2Adressing = Register;
				reg2 = currentWord[1] - '0';
			}
			else
			{
				if (!isValidLabel(currentWord))
				{
					printf("ERROR: Illegal parameter (LINE %d).\n", lineNum);
					lineError = FAIL;
					return FALSE;
				}
				param2Adressing = Label;
				addLabelOperands(labelOperandsList, currentWord, ic + 3, lineNum);
			}
			/**/

			pt = skipUntilSign(pt, ')');
			if (isspace(*pt))
			{

				printf("ERROR: Structure of command incorrect (LINE %d).\n", lineNum);
				lineError = FAIL;
				return FALSE;
			}
			else
			{
				pt++;
				pt = skipSpaces(pt);
				if (!endOfLine(pt))
				{
					printf("ERROR: Extraneous text after end of command (LINE %d).\n", lineNum);
					lineError = FAIL;
					return FALSE;
				}
				else
				{
					targetOpAdressing = JumpWithParam;
				}
			}
		}
	}
	codeRegister(codeArr, ic + 1, reg1, reg2);
	return TRUE;
}

/* This function validates a label name. */
/* Returns TRUE if the word is valid. Else - FALSE*/
boolean isValidLabel(char *word)
{
	int wordLen = strlen(word);
	int i;

	if (word == NULL)
		return FALSE;
	if (wordLen > LABEL_LENGTH)
		return FALSE;
	if (!isalpha(word[0]))
		return FALSE;
	if (isCommand(word))
		return FALSE;
	if (isRegister(word))
		return FALSE;
	if (isDirectiveName(word))
		return FALSE;

	for (i = 0; i < wordLen; i++)
	{
		if (!isalpha(word[i]) && !isdigit(word[i]))
			return FALSE;
	}
	return TRUE;
}

/* This function checks if the word is a label definition. */
/* Returns TRUE if the word is a label definition. Else - FALSE*/
boolean isLabelDefinition(char *word)
{
	int wordLen = strlen(word);
	int i;

	if (word == NULL)
	{
		return FALSE;
	}

	if (wordLen > LABEL_LENGTH)
	{
		printf("ERROR: Lable length must be lower than %d (LINE %d).\n", LABEL_LENGTH, lineNum);
		lineError = FAIL;
		return FALSE;
	}

	if (!isalpha(word[0]))
	{
		printf("ERROR: Lable must start with a letter (LINE %d).\n", lineNum);
		lineError = FAIL;
		return FALSE;
	}

	if (isCommand(word))
	{
		printf("ERROR: Lable can't be similar to command (LINE %d).\n", lineNum);
		lineError = FAIL;
		return FALSE;
	}
	if (isRegister(word))
	{
		printf("ERROR: Lable can't be similar to register name (LINE %d).\n", lineNum);
		lineError = FAIL;
		return FALSE;
	}

	if (isDirectiveName(word))
	{
		printf("ERROR: Lable can't be similar to directive name (LINE %d).\n", lineNum);
		lineError = FAIL;
		return FALSE;
	}

	for (i = 0; i < wordLen - 1; i++)
	{
		if (!isalpha(word[i]) && !isdigit(word[i]))
		{
			printf("ERROR: Lable cant have non-digit and non-alphabetic characters (LINE %d).\n", lineNum);
			lineError = FAIL;
			return FALSE;
		}
	}

	return TRUE;
}

/* This function checks if the word is a directive name. */
/* Returns TRUE if the word is a directive name. Else - FALSE*/
boolean isDirectiveName(char *word)
{
	int i;

	for (i = 0; i < NUM_OF_DATA_DIRECTIVES + NUM_OF_LABEL_DIRECTIVES; i++)
	{
		if (strcmp(word, directiveNames[i]) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}

/* This function checks if the word is a register name. */
/* Returns TRUE if the word is a register name. Else - FALSE*/
boolean isRegister(char *word)
{
	if (strlen(word) == 2 && word[0] == 'r' && word[1] >= '0' && word[1] <= '7')
	{
		return TRUE;
	}

	return FALSE;
}

/* This function checks if the word is a command name. */
/* Returns the index of the command if the word is a command name. Else - 0*/ 
int isCommand(char *word)
{
	const char *command_names[NUM_OF_COMMANDS] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
	int i;

	for (i = 0; i < NUM_OF_COMMANDS; i++)
	{
		if (strcmp(word, command_names[i]) == 0)
		{
			return i + 1;
		}
	}

	return 0;
}

/* This function returns the immediate value of a word. */
int immediateValue(char *word)
{
	int num;
	if (word[1] == '+')
		num = atoi(word + 2);
	else if (word[1] == '-')
	{
		num = atoi(word + 2);
		num = -num;
	}
	else
		num = atoi(word + 1);

	return num;
}

/* This function checks if the word is an immediate value. */
/* Returns TRUE if the word is an immediate value. Else - FALSE*/
boolean isImmediate(char *word)
{
	int i;
	if (word[0] != '#')
		return FALSE;
	if (word[1] != '+' && word[1] != '-' && (!isdigit(word[1])))
		return FALSE;
	for (i = 2; word[i] != '\0' && word[i] != '\n'; i++)
	{
		if (!isdigit(word[i]))
			return FALSE;
	}
	return TRUE;
}

/* This function checks if the word is a data directive. */
/* Returns TRUE if the word is a data directive. Else - FALSE*/
boolean isDataDirective(char *word)
{
	int i;

	for (i = 0; i < NUM_OF_DATA_DIRECTIVES; i++)
	{
		if (strcmp(word, dataDirectives[i]) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}

/* This function checks if the word is a label directive. */
/* Returns TRUE if the word is a label directive. Else - FALSE*/
boolean isLabelDirective(char *word)
{
	int i;

	for (i = 0; i < NUM_OF_LABEL_DIRECTIVES; i++)
	{
		if (strcmp(word, labelDirectives[i]) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}

/* This function returns the number of the command group of the word. */
/* 1 for TwoArg, 2 for OneArg, and 3 for ZeroArg*/
int whichCommandGroup(char *word)
{
	int i;

	for (i = 0; i < NUM_OF_COMMANDS_FIRST_GROUP; i++)
	{
		if (strcmp(word, firstGroupCommands[i]) == 0)
		{
			return TwoArg;
		}
	}

	for (i = 0; i < NUM_OF_COMMANDS_SECOND_GROUP; i++)
	{
		if (strcmp(word, secondGroupCommands[i]) == 0)
		{
			return OneArg;
		}
	}

	for (i = 0; i < NUM_OF_COMMANDS_THIRD_GROUP; i++)
	{
		if (strcmp(word, thirdGroupCommands[i]) == 0)
		{
			return ZeroArg;
		}
	}

	return 0;
}

/* This function checks if the word is a jump command. */
/* Returns TRUE if the word is a jump command. Else - FALSE*/
boolean isJumpCommand(char *word)
{
	int i;

	for (i = 0; i < NUM_OF_JWP; i++)
	{
		if (strcmp(word, jumpWithParameters[i]) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}

/* This function checks if the add. */
/*return TRUE if the adressing system is valid with the command name else return FALSE*/
boolean isValidAdressingSystem(adressingSystem system, commandName cmd, int isSource) /* 1 for source 0 for target*/
{
	if (isSource == 1)
	{
		if (system == Immediate && cmd == Lea)
		{
			printf("ERROR: Can't have immidiate as source operand (LINE %d).\n", lineNum);
			lineError = FAIL;
			return FALSE;
		}
		if (system == Register && cmd == Lea)
		{
			printf("ERROR: Can't have register as source operand (LINE %d).\n", lineNum);
			lineError = FAIL;
			return FALSE;
		}
	}
	else
	{
		if (system == Immediate && cmd != Prn && cmd != Cmp)
		{
			printf("ERROR: Can't have immidiate as target operand (LINE %d).\n", lineNum);
			lineError = FAIL;
			return FALSE;
		}
	}
	return TRUE;
}

/* This function skips every character untill comma is appeared. */
/* Returns the pointer to the next character after the comma. */
/* If the comma is not found, returns NULL. */
char *skipComma(char *ch)
{
	if (ch == NULL)
		return NULL;

	while (!isspace(*ch) && ((*ch) != ','))
		ch++;

	if (isspace(*ch))
	{
		ch = skipSpaces(ch);
		if (endOfLine(ch))
		{
			printf("ERROR: Missing target operand (LINE %d).\n", lineNum);
			lineError = FAIL;
		}
		else if ((*ch) != ',')
		{
			printf("ERROR: Missing comma (LINE %d).\n", lineNum);
			lineError = FAIL;
		}
		else
		{
			ch = skipSpaces(ch + 1);
			return ch;
		}
	}
	else
	{
		ch = skipSpaces(ch + 1);
		return ch;
	}
	return NULL;
}

/* This function joins the code and data arrays into the code array: */
void joinDataAndCommand(int codeArr[], int dataArr[])
{
	int i;
	for (i = 0; i < dc; i++)
	{
		codeArr[ic - MEMORY_LOAD_START + i] = dataArr[i];
	}
}

/* This function updates the ic by the number of words in the current command coding. */
void updateIc()
{
	ic++; /*every command takes atleast one word*/

	if (targetOpAdressing == NoOperand && sourceOpAdressing == NoOperand)
		;
	else if (targetOpAdressing == JumpWithParam)
	{
		ic++;
		if (param1Adressing == Register && param2Adressing == Register)
			ic += 1;
		else
			ic += 2;
	}
	else if (targetOpAdressing == NoOperand || sourceOpAdressing == NoOperand)
		ic += 1;
	else if (targetOpAdressing == Register && sourceOpAdressing == Register)
		ic += 1;
	else
		ic += 2;
}
