#include "parse_argument.h"
#include <stdio.h>
#include <stdbool.h>

extern char *inputFile;
extern char *outputFile;
extern bool printPrefixTable;


void printSyntax(void)
{
	printf("Command syntax:\n");
	printf("chcmpss <inputFile> <outputFile> : compress inputFile to outputFile\n");
	printf("chcmpss -p <inputFile> : print prefixTable\n");
}

int parseArgument(int argc, char **argv)
{
	if (argc == 1 || argc == 2) {
		printSyntax();
		return PA_ERROR;
	}
	if (argc != 3) {
			printSyntax();
			return PA_ERROR;
	}
	if (argv[1][0] != '-') {
		outputFile = argv[2];
		inputFile = argv[1];
		return PA_SUCCESS;
	}
	if (argv[1][1] == 'p') {
		printPrefixTable = true;
		inputFile = argv[2];
		return PA_SUCCESS;
	}
	printSyntax();
	return PA_ERROR;
}
