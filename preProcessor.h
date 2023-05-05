/* ------------------------------------- */
/*               Yoav Levy               */
/*                                       */
/*            2023 Semester A            */
/* ------------------------------------- */

#include "utils.h"

/* macro chart */
struct object1
{
	char *name;
	char *lines;
	struct object1 *next;
};
typedef struct object1 macro;

/* This function spreads the macros */
int spreadMacros(FILE *fd, FILE *fd2);

/* This function checks if macro's name is valid */
int nameValid(char name[]);
int is_valid2(char line[], char word[]);

/* This function checks if we are looking for endmcr */
int whichKind(char line[], char name[]);

/* This function checks what is the number of the macro, given by its name */
int which_mcr(char word[], macro *head, FILE *dest);

/* This function clears the macros linked list */
int freeList(macro *head);

enum
{
	MCR = 1,
	ENDMCR
};

/* Checks if a word is a register: */
boolean isRegister(char *word);

/* Checks if a word is a command, also returns the opcode of the command plus one (opcode+1), if not a command return 0 */
int isCommand(char *word);
