/* ------------------------------------- */
/*               Yoav Levy               */
/*                                       */
/*            2023 Semester A            */
/* ------------------------------------- */

#include "assembler.h"

/* The main function of the assembler. */
int main(int argc, char *argv[])
{
	/* Checking if there are any file names: */
	if (argc == 1)
		printf("no file names were entered! program finished!");
	else
	{
		/* Setting up variables: */
		int ind = 1;			  /* the index of the file we are working on */
		isFail error_found;		  /* If any errors found during the first or second pass */
		int codeArr[MEMORY_SIZE]; /* The code array */
		int dataArr[MEMORY_SIZE]; /* The data array */
		int length[2];			  /* The lengths of the code and data arrays */

		label *labelsList;
		labelOperands *labelOperandsList;
		externLabel *externList;
		entryLabel *entryList;

		/* Running the assembler on all the files: */
		while (ind < argc)
		{
			/* Resetting the variables: */
			error_found = 0;
			resetArray(codeArr);
			resetArray(dataArr);

			printf("\nRunning for file number: %d, name: %s.as.\n\n", ind, argv[ind]);

			/* Pre-assembler proccess: */
			if (preAssembler(argv[ind]))
				/* If there are errors in the pre-assembler proccess, the assembler will not run */
				error_found = FAIL;

			if (!error_found) /* if no errors in the pre-assembler proccess*/
			{
				/* Resetting the variables: */
				length[0] = 0;
				length[1] = 0;
				labelsList = NULL;
				labelOperandsList = NULL;
				externList = NULL;
				entryList = NULL;

				/* Calling the first pass */
				error_found = callFirstPass(argv[ind], codeArr, dataArr, &labelsList, &labelOperandsList, &entryList, length);

				/* Calling the second pass */
				error_found = secondPass(argv[ind], codeArr, labelsList, labelOperandsList, externList, entryList, error_found, length);

				/* Freeing the linked lists */
				freeLabels(labelsList);
				freeLabelOperands(labelOperandsList);
				freeLabelExtern(externList);
				freeLabelEntry(entryList);

				if (error_found == FAIL)
					printf("\nThere were errors in the file %s.as.\nNo output files were generated!\n", argv[ind]);
				else if(error_found == SUCCESS)
					printf("\nThe file %s.as was compiled successfully.\nOutput files were generated.\n", argv[ind]);
			}

			ind++; /* move to the next file name */
		}
	}

	return 0;
}

/* This function resets the array to 0 */
void resetArray(int arr[])
{
	int i;
	for (i = 0; i < MEMORY_SIZE; i++)
	{
		arr[i] = 0;
	}
}

/* This function calls the first pass. */
int callFirstPass(char filename[], int codeArr[], int dataArr[], label **labelsList, labelOperands **labelOperandsList, entryLabel **entryList, int length[])
{
	/* Setting up variables: */
	FILE *read_from;
	char filename_new[FILENAME_MAX];

	/* Opening the file to read from: */
	strcpy(filename_new, filename); /*get the name of the file to read from*/
	strcat(filename_new, ".am");
	read_from = fopen(filename_new, "r");

	/* Checking if the file opened correctly: */
	if (read_from == NULL) /* file didnt open correctly */
	{
		printf("Can't open %s!\n", filename_new);
		return FAIL;
	}

	/* Checking if file is empty*/
	fseek(read_from, 0, SEEK_END);

	if (ftell(read_from) == 0) /* if the file is empty */
	{
		printf("%s is empty!\n", filename_new);
		fclose(read_from);
		return -1;
	}

	rewind(read_from);

	/* Calling the first pass function: */
	return firstPass(read_from, codeArr, dataArr, labelsList, labelOperandsList, entryList, length);
}
