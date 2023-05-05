/* ------------------------------------- */
/*               Yoav Levy               */
/*                                       */
/*            2023 Semester A            */
/* ------------------------------------- */

#include "secondPass.h" 

isFail error;

/* This function makes the second pass. */
isFail secondPass(char filename[], int codeArr[], label *labelsList, labelOperands *labelOperandsList, externLabel *externList, entryLabel *entryList, isFail isError, int length[])
{
	/* Setting up variables: */
	labelOperands *temp = labelOperandsList;
	label *temp2;
	entryLabel *temp3 = entryList;
	error = isError;

	/* Going through every label that needs to be written in the code array: */
	while (temp != NULL)
	{
		/* Checking if the label exists: */
		temp2 = doesLabelExist(labelsList, temp);
		if (temp2 == NULL)
		{
			/* If the label doesnt exist, print an error message and set isError to FAIL: */
			printf("ERROR: Label %s doesnt exist (LINE %d).\n", temp->name, temp->line);
			error = FAIL;
		}
		else
		{
			/* If the label exists, code it to the code array: */
			codeLabel(codeArr, temp2, temp->address);

			/* If the label is extern, add it to the externs list: */
			if (temp2->is_extern)
				addLabelToExterns(&externList, temp);
		}

		temp = temp->next;
	}

	/* Going through every entry label: */
	while (temp3 != NULL)
	{
		/* Checking if the label exists: */
		temp2 = doesEntryLabelExist(labelsList, temp3);

		if (temp2 == NULL)
		{
			/* If the label doesnt exist, print an error message and set error to FAIL: */
			printf("ERROR: Label %s doesnt exist (LINE %d).\n", temp3->name, temp3->line);
			error = FAIL;
		}

		temp3 = temp3->next;
	}

	/* If there were no errors, generate the final files: */
	if (error == SUCCESS)
	{
		generateObjectFile(filename, codeArr, length);
		generateExternFile(filename, externList);
		generateEntryFile(filename, entryList, labelsList);
	}

	return error;
}

/* This function checks if an operand label exists in the labels list. */
/* If it does, it returns a pointer to the label. */
/* If it doesnt, it returns NULL. */
label *doesLabelExist(label *labelsList, labelOperands *labelOperandsList)
{

	while (labelsList != NULL)
	{
		if (!(strcmp(labelsList->name, labelOperandsList->name)))
			return labelsList;

		labelsList = labelsList->next;
	}

	return NULL;
}

/* This function checks if an entry label exists in the labels list. */
/* If it does, it returns a pointer to the label. */
/* If it doesnt, it returns NULL. */
label *doesEntryLabelExist(label *labelsList, entryLabel *entryList)
{

	while (labelsList != NULL)
	{
		if (!(strcmp(labelsList->name, entryList->name)))
			return labelsList;

		labelsList = labelsList->next;
	}

	return NULL;
}

/* This function returns a pointer to an entry label in the labels list. */
label *getEntryLabel(label *labelsList, char *entryLabelname)
{
	label *temp = labelsList;

	while (temp != NULL)
	{
		if (strcmp(temp->name, entryLabelname) == 0)
			return temp;

		temp = temp->next;
	}

	return NULL;
}

/* This function adds a label to the externs list. */
void addLabelToExterns(externLabel **head, labelOperands *labelToAdd)
{
	/* Setting up variables: */
	externLabel *newLabel = NULL;
	externLabel *temp;

	/* Allocating memory for the new label: */
	newLabel = (externLabel *)malloc(sizeof(externLabel));

	/* Checking if malloc has failed: */
	if (newLabel == NULL)
	{
		/* If it has, print an error message and return: */
		printf("ERROR: Malloc has failed while trying to allocate memory for label.\n");
		error = FAIL;
		return;
	}

	/* Setting up the new label: */
	strcpy(newLabel->name, labelToAdd->name);
	newLabel->address = labelToAdd->address;
	newLabel->next = NULL;

	/* If the list is empty, add the new label to the head of the list: */
	if (*head == NULL)
	{
		*head = newLabel;
		return;
	}

	/* If the list is not empty, add the new label to the end of the list: */
	temp = *head;

	while (temp->next != NULL)
	{
		temp = temp->next;
	}

	temp->next = newLabel;

	return;
}

/* This function codes a label its appropriate index in the code. */
void codeLabel(int arr[], label *l, int ic)
{
	int ind = ic - MEMORY_LOAD_START;

	/* Shifting the address to bits 2-14: */
	int mask = (l->address) << LABEL_START_BIT;

	/* Setting the ARE bits - (2) 10 by default: */
	int are = 2;

	/* If the label is extern, set ARE to be (1) 01: */
	if (l->is_extern)
		are = 1;

	/* Adding the ARE bits to the final 14 bits word*/
	mask = mask | are;

	/* Adding the word to the code array: */
	arr[ind] = arr[ind] | mask;
}

/* This function generates the .ob file. */
void generateObjectFile(char filename[], int codeArr[], int length[])
{
	/* Setting up variables: */
	FILE *fp;
	int number;
	int mask, i, j;
	char *ext;
	char *newFileName;
	ext = ".ob";

	/* Allocating memory for the new file name: */
	newFileName = (char *)malloc(strlen(filename) + strlen(ext) + 1);

	/* Copying the name of the file: */
	strcpy(newFileName, filename);

	/* Adding the .ob extention */
	strcat(newFileName, ext);

	/* Opening the file: */
	fp = fopen(newFileName, "w");

	/* Checking if fopen has failed: */
	if (fp == NULL)
	{
		printf("Can't open %s!\n", newFileName);
		return;
	}

	/* Writing the first line of the file: */
	fprintf(fp, "\t%d  %d", length[0], length[1]);

	/* Writing the code to the file: */
	for (i = 0; i < length[0] + length[1]; i++)
	{
		/* Writing the address, with leading zeros: */
		fprintf(fp, "\n%04d\t", i + MEMORY_LOAD_START);

		/* Masking the number */
		number = codeArr[i];
		mask = 1 << 13;

		/* Writing the number to the file, with the uniqe binary: */
		for (j = 0; j < 14; j++)
		{
			if ((mask & number) != 0)
				fprintf(fp, "/");
			else
				fprintf(fp, ".");
			mask = mask >> 1;
		}
	}

	/* Closing the file: */
	fclose(fp);
}

/* This function generates the .ext file. */
void generateExternFile(char filename[], externLabel *externList)
{
	/* Setting up variables: */
	FILE *fp;
	char *ext;
	char *newFileName;

	/* Checking if the list is empty: */
	if (externList == NULL)
		return;

	ext = ".ext";

	/* Allocating memory for the new file name: */
	newFileName = (char *)malloc(strlen(filename) + strlen(ext) + 1);

	/* Copying the name of the file: */
	strcpy(newFileName, filename);

	/* Adding the .ext extention */
	strcat(newFileName, ext);

	/* Opening the file: */
	fp = fopen(newFileName, "w");

	/* Checking if fopen has failed: */
	if (fp == NULL)
	{
		printf("Can't open %s!\n", newFileName);
		return;
	}

	/* Writing the externs to the file: */
	while (externList != NULL)
	{
		fprintf(fp, "%s\t%d\n", externList->name, externList->address);
		externList = externList->next;
	}

	/* Closing the file: */
	fclose(fp);
}

/* This function generates the .ent file. */
void generateEntryFile(char filename[], entryLabel *entryList, label *labelsList)
{
	/* Setting up variables: */
	FILE *fp;
	char *ext;
	char *newFileName;
	label *temp;

	/* Checking if the list is empty: */
	if (entryList == NULL)
		return;

	ext = ".ent";

	/* Allocating memory for the new file name: */
	newFileName = (char *)malloc(strlen(filename) + strlen(ext) + 1);

	/* Copying the name of the file: */
	strcpy(newFileName, filename);

	/* Adding the .ent extention */
	strcat(newFileName, ext);

	/* Opening the file: */
	fp = fopen(newFileName, "w");

	/* Checking if fopen has failed: */
	if (fp == NULL)
	{
		printf("Can't open %s!\n", newFileName);
		return;
	}

	/* Writing the entries to the file: */
	while (entryList != NULL)
	{
		temp = getEntryLabel(labelsList, entryList->name);

		fprintf(fp, "%s\t%d\n", temp->name, temp->address);
		entryList = entryList->next;
	}

	/* Closing the file: */
	fclose(fp);
}
