/* ------------------------------------- */
/*               Yoav Levy               */
/*                                       */
/*            2023 Semester A            */
/* ------------------------------------- */

#include "labelFunc.h"

/* The Pre Assembler function */
int preAssembler(char filename[]);

/* This functions are for calling the first pass: */
int callFirstPass(char filename[], int codeArr[], int dataArr[], label **labelsList, labelOperands **labelOperandsList, entryLabel **entryList, int length[]);
isFail firstPass(FILE *fp, int codeArr[], int dataArr[], label **labelsList, labelOperands **labelOperandsList, entryLabel **entryList, int length[]);

/* This function is for calling the second pass: */
isFail secondPass(char filename[], int codeArr[], label *labelsList, labelOperands *labelOperandsList, externLabel *externList, entryLabel *entryList, isFail isError, int length[]);

/* This function resets an array at the assembler.c file: */
void resetArray(int arr[]);
