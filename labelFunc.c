/* ------------------------------------- */
/*               Yoav Levy               */
/*                                       */
/*            2023 Semester A            */
/* ------------------------------------- */

#include "labelFunc.h"

/* This function adds a new label to the labels linked list. */
/* Returns FALSE if there is another label with that name. Else - FALSE*/
boolean addLabel(label **head, char *name, int address, boolean is_extern, boolean is_entry, boolean is_data)
{
	/* Setting up varibales */
	label *newLabel = NULL;
	label *temp;

	/* Checking if the label already exists: */
	if (isLabelExists(*head, name))
	{
		return FALSE;
	}

	/* Allocating memory for the new label: */
	newLabel = (label *)malloc(sizeof(label));

	/* Allocating memory for the new label: */
	if (newLabel == NULL)
	{
		printf("ERROR: Malloc has failed while trying to allocate memory for label.\n");

		return FALSE;
	}

	/* Setting up the new label: */
	strcpy(newLabel->name, name);
	newLabel->address = address;
	newLabel->is_extern = is_extern;
	newLabel->is_entry = is_entry;
	newLabel->is_data = is_data;
	newLabel->next = NULL;

	/* If the list is empty, add the new label to the head of the list: */
	if (*head == NULL)
	{
		*head = newLabel;
		return TRUE;
	}

	temp = *head;

	/* If the list is not empty, add the new label to the end of the list: */
	while (temp->next != NULL)
	{
		temp = temp->next;
	}

	temp->next = newLabel;

	return TRUE;
}

/* This function adds a new operand label to the operand labels linked list. */
/* Returns FALSE if there is another label with that name. Else - FALSE*/
boolean addLabelOperands(labelOperands **head, char *name, int address, int lineNum)
{
	/* Setting up varibales */
	labelOperands *newLabel = NULL;
	labelOperands *temp;

	/* Allocating memory for the new label: */
	newLabel = (labelOperands *)malloc(sizeof(labelOperands));

	/* Allocating memory for the new label: */
	if (newLabel == NULL)
	{
		printf("ERROR: Malloc has failed while trying to allocate memory for label.\n");

		return FALSE;
	}

	/* Setting up the new label: */
	strcpy(newLabel->name, name);
	newLabel->address = address;
	newLabel->line = lineNum;
	newLabel->next = NULL;

	/* If the list is empty, add the new label to the head of the list: */
	if (*head == NULL)
	{
		*head = newLabel;
		return TRUE;
	}

	temp = *head;

	/* If the list is not empty, add the new label to the end of the list: */
	while (temp->next != NULL)
	{
		temp = temp->next;
	}

	temp->next = newLabel;

	return TRUE;
}

/* This functions returns a label by its name. */
/* Returns the label, if found. Else - NULL*/
label *getLabel(label *head, char *name)
{
	label *temp = head;
	while (temp != NULL)
	{
		if (strcmp(temp->name, name) == 0)
		{
			return temp;
		}
		temp = temp->next;
	}
	return NULL;
}

/* This function checks if a label already exists, by name. */
/* Returns TRUE if the label exists. Else - FALSE*/
boolean isLabelExists(label *head, char *name)
{
	return getLabel(head, name) != NULL;
}

/* This function frees the labels linked list. */
void freeLabels(label *head)
{
	label *temp = head;
	while (temp != NULL)
	{
		head = temp->next;
		free(temp);
		temp = head;
	}
}

/* This function frees the operand labels linked list. */
void freeLabelOperands(labelOperands *head)
{
	labelOperands *temp = head;
	while (temp != NULL)
	{
		head = temp->next;
		free(temp);
		temp = head;
	}
}

/* This function frees the external labels linked list. */
void freeLabelExtern(externLabel *head)
{
	externLabel *temp = head;
	while (temp != NULL)
	{
		head = temp->next;
		free(temp);
		temp = head;
	}
}

/* This function frees the entry labels linked list. */
void freeLabelEntry(entryLabel *head)
{
	entryLabel *temp = head;
	while (temp != NULL)
	{
		head = temp->next;
		free(temp);
		temp = head;
	}
}

/* This function adds an offset the data's labels address */
void offsetDataLabels(label *head, int offset)
{
	label *temp = head;
	while (temp != NULL)
	{
		if (temp->is_data)
		{
			temp->address += offset;
		}
		temp = temp->next;
	}
}

/* This function prints a label linked list. */
void printList(label *node)
{
	while (node != NULL)
	{
		printf("Name: %s, Address: %d, is_extern: %d, is_entry: %d, is_data: %d\n", node->name, node->address, node->is_extern, node->is_entry, node->is_data);
		node = node->next;
	}
}

/* This function prints an operand label linked list. */
void printOperandsList(labelOperands *node)
{
	while (node != NULL)
	{
		printf("Name: %s, Address: %d\n", node->name, node->address);
		node = node->next;
	}
}

/* This function prints an entry label linked list. */
void printEntryList(entryLabel *node)
{
	while (node != NULL)
	{
		printf("ENTRY Name: %s\n", node->name);
		node = node->next;
	}
}

/* This function adds a new label to the entry labels linked list. */
void addLabelEntry(entryLabel **head, char *name, int line)
{
	/* Setting up varibales */
	entryLabel *newLabel = NULL;
	entryLabel *temp;

	/* Allocating memory for the new label: */
	newLabel = (entryLabel *)malloc(sizeof(entryLabel));

	/* Allocating memory for the new label: */
	if (newLabel == NULL)
	{
		printf("ERROR: Malloc has failed while trying to allocate memory for label.\n");

		return;
	}

	/* Setting up the new label: */
	strcpy(newLabel->name, name);
	newLabel->line = line;
	newLabel->next = NULL;

	/* If the list is empty, add the new label to the head of the list: */
	if (*head == NULL)
	{
		*head = newLabel;
		return;
	}

	temp = *head;

	/* If the list is not empty, add the new label to the end of the list: */
	while (temp->next != NULL)
	{
		temp = temp->next;
	}

	temp->next = newLabel;

	return;
}
