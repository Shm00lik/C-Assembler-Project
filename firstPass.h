/* ------------------------------------- */
/*               Yoav Levy               */
/*                                       */
/*            2023 Semester A            */
/* ------------------------------------- */

#include "labelFunc.h"

/* Function to process every line */
void processLine(char *line, int codeArr[], int dataArr[], label **labelsList, labelOperands **labelOperandsList, entryLabel **entryList);

/* Functions that checks if a word is equals to something: */
boolean isLabelDefinition(char *word);
boolean isValidLabel(char *word);

/* Checks if a word is a register */
boolean isRegister(char *word);

/* Checks if a word is a command, also returns the opcode of the command plus one (opcode+1), if not a command return 0 */
int isCommand(char *word);
boolean isImmediate(char *word);
boolean isJumpCommand(char *word);
int whichCommandGroup(char *word);
boolean isLabelDirective(char *word);
boolean isDataDirective(char *word);
boolean isDirectiveName(char *word);

/* Functions that checks things about the adressing system: */
boolean twoArgAdressing(char *line, char *currentWord, int codeArr[], labelOperands **labelOperandsList);
boolean oneArgAdressing(char *line, char *currentWord, int codeArr[], labelOperands **labelOperandsList);
boolean zeroArgAdressing(char *line, char *currentWord);
boolean JumpAdressing(char *line, char *currentWord, int codeArr[], labelOperands **labelOperandsList);
boolean isValidAdressingSystem(adressingSystem system, commandName cmd, int isSource);
int immediateValue(char *word);

/* This function skips every character untill comma is appeared. */
char *skipComma(char *ch);

/* Functions that handles the directives: */
void handleLabelDirective(char *line, char *currentWord, label **labelsList, entryLabel **entryList);
void handleDataDirective(char *line, char *currentWord, int dataArr[]);

/* Other functions */
void updateIc();
void joinDataAndCommand(int codeArr[], int dataArr[]);

/* Functions that codes a word to its binary representation */
void codeFirstWord(int codeArr[], commandName cmd, int ic, adressingSystem targetOpAdressing, adressingSystem sourceOpAdressing, adressingSystem param1Adressing, adressingSystem param2Adressing);
void codeImmediate(int codeArr[], int ic, int num);
void codeRegister(int codeArr[], int ic, int reg1, int reg2);
