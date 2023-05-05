/* ------------------------------------- */
/*               Yoav Levy               */
/*                                       */
/*            2023 Semester A            */
/* ------------------------------------- */

#include "utils.h"

/* Defining the label type: */
struct object2
{
	char name[LABEL_LENGTH + 1];
	int address;
	boolean is_extern;
	boolean is_entry;
	boolean is_data;
	struct object2 *next;
};
typedef struct object2 label;

/* Defining the labelOperands type: */
struct object3
{
	char name[LABEL_LENGTH + 1];
	int address;
	int line;
	struct object3 *next;
};
typedef struct object3 labelOperands;

/* Defining the entryLabel type: */
struct object4
{
	char name[LABEL_LENGTH + 1];
	int line;
	struct object4 *next;
};
typedef struct object4 entryLabel;

/* Defining the externLabel type: */
struct object5
{
	char name[LABEL_LENGTH + 1];
	int address;
	struct object5 *next;
};
typedef struct object5 externLabel;

/* Functions that deals with the labels linked list: */
boolean addLabel(label **head, char *name, int address, boolean is_extern, boolean is_entry, boolean is_data);
label *getLabel(label *head, char *name);
boolean isLabelExists(label *head, char *name);
void freeLabels(label *head);
void offsetDataLabels(label *head, int offset);
void printList(label *head);

/* Functions that deals with the operands labels linked list: */
boolean addLabelOperands(labelOperands **head, char *name, int address, int lineNum);
void freeLabelOperands(labelOperands *head);
void printOperandsList(labelOperands *node);

/* Functions that deals with the extern labels linked list: */
void freeLabelExtern(externLabel *head);

/* Functions that deals with the entry labels linked list: */
void addLabelEntry(entryLabel **head, char *name, int line);
void freeLabelEntry(entryLabel *head);
void printEntryList(entryLabel *node);
