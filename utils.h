/* ------------------------------------- */
/*               Yoav Levy               */
/*                                       */
/*            2023 Semester A            */
/* ------------------------------------- */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/* Defining constants */
#define LINE_LENGTH 82
#define LABEL_LENGTH 30

#define WORD_SIZE 14
#define MEMORY_SIZE 256
#define MEMORY_LOAD_START 100

#define NUM_OF_COMMANDS_FIRST_GROUP 5
#define NUM_OF_COMMANDS_SECOND_GROUP 9
#define NUM_OF_COMMANDS_THIRD_GROUP 2
#define NUM_OF_JWP 3

#define NUM_OF_COMMANDS 16
#define NUM_OF_REG 8

#define NUM_OF_DATA_DIRECTIVES 2
#define NUM_OF_LABEL_DIRECTIVES 2

/* Defining the boolean type: */
typedef enum
{
	FALSE,
	TRUE
} boolean;

/* Defining the isFail type: */
typedef enum
{
	SUCCESS,
	FAIL
} isFail;

#define NO_REG -1

/* This enum contains all the commands: */ 
typedef enum
{
	Mov = 1,
	Cmp,
	Add,
	Sub,
	Not,
	Clr,
	Lea,
	Inc,
	Dec,
	Jmp,
	Bne,
	Red,
	Prn,
	Jsr,
	Rts,
	Stop
} commandName;

/* This enum contains all the commands groups: */
typedef enum
{
	TwoArg = 1,
	OneArg,
	ZeroArg
} commandGroup;

/* This enum contains all the adressing systems types: */
typedef enum
{
	Immediate = 0,
	Label,
	JumpWithParam,
	Register,
	NoOperand
} adressingSystem;


/* Help functions: */
boolean endOfLine(char *line);
char *skipSpaces(char *ch);
char *skipNotSpaces(char *ch);
char *skipUntilSign(char *ch, char sign);
void copyUntilSign(char *line, char *to, char sign);
int ignoreLine(char *line);
void copyNextWord(char *line, char *to);
void copyNextOperandName(char *line, char *to);




