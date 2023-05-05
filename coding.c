/* ------------------------------------- */
/*               Yoav Levy               */
/*                                       */
/*            2023 Semester A            */
/* ------------------------------------- */

#include "coding.h"

/* This function codes the first word of any instruction, to the code array. */
void codeFirstWord(int arr[], commandName cmd, int ic, adressingSystem targetOpAdressing, adressingSystem sourceOpAdressing, adressingSystem param1Adressing, adressingSystem param2Adressing)
{
	int mask, ind;
	ind = ic - MEMORY_LOAD_START;

	mask = (cmd - 1) << OPCODE_START_BIT;

	arr[ind] = arr[ind] | mask;

	if (targetOpAdressing != NoOperand)
	{
		mask = targetOpAdressing << TARGET_OP_START_BIT;
		arr[ind] = arr[ind] | mask;
	}

	if (sourceOpAdressing != NoOperand)
	{
		mask = sourceOpAdressing << SOURCE_OP_START_BIT;
		arr[ind] = arr[ind] | mask;
	}

	if (targetOpAdressing == JumpWithParam)
	{
		mask = param1Adressing << PARAM1_START_BIT;
		arr[ind] = arr[ind] | mask;

		mask = param2Adressing << PARAM2_START_BIT;
		arr[ind] = arr[ind] | mask;
	}
}

/* This function codes an immediate operand / parameter to the code array. */
void codeImmediate(int arr[], int ic, int num)
{
	int mask = num << IMMEDIATE_START_BIT;
	int ind = ic - MEMORY_LOAD_START + 1;
	arr[ind] = arr[ind] | mask;
}

/* This function codes an register operand / parameter to the code array. */
void codeRegister(int arr[], int ic, int reg1, int reg2)
{
	int mask, ind;
	ind = ic - MEMORY_LOAD_START + 1;

	if (reg1 != NO_REG && reg2 != NO_REG)
	{
		mask = reg1 << REG1_START_BIT;
		arr[ind] = arr[ind] | mask;
		mask = reg2 << REG2_START_BIT;
		arr[ind] = arr[ind] | mask;
	}
	else if (reg1 != NO_REG)
	{
		mask = reg1 << REG1_START_BIT;
		arr[ind] = arr[ind] | mask;
	}
	else if (reg2 != NO_REG)
	{
		mask = reg2 << REG2_START_BIT;
		arr[ind + 1] = arr[ind + 1] | mask;
	}
}
