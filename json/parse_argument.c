#include "parse_argument.h"
#include <stdio.h>

int parseArgument(int argc, char **argv)
{
	if (argc == 1) {
		printf("Input file not defined\n");
		return PA_ERROR;
	}
	if (argc != 2) {
			printf("Too many parameters\n");
			return PA_ERROR;
	}
	return PA_SUCCESS;
}