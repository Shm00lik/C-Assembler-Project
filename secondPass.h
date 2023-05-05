/* ------------------------------------- */
/*               Yoav Levy               */
/*                                       */
/*            2023 Semester A            */
/* ------------------------------------- */

#include "coding.h"

/* Functions that deals with the labels linked lists: */
label *doesLabelExist(label *labelsList, labelOperands *labelOperandsList);
label *doesEntryLabelExist(label *labelsList, entryLabel *entryList);
label *getEntryLabel(label *labelsList, char *name);
void addLabelToExterns(externLabel **head, labelOperands *labelToAdd);

/* Functions that generates the final files: */
void generateObjectFile(char filename[], int codeArr[], int length[]);
void generateExternFile(char filename[], externLabel *labelsList);
void generateEntryFile(char filename[], entryLabel *entryList, label *labelsList);

/* This function codes a label to the code array: */
void codeLabel(int arr[], label *l, int ic);

