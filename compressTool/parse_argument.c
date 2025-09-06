#include "parse_argument.h"
#include <stdio.h>
#include <stdbool.h>

extern char *inputFile;
extern char *outputFile;
extern bool printPrefixTable;
extern bool readPrefixTable;


void printSyntax(void)
{
	printf("Command syntax:\n");
	printf("chcmpss -h: print this message\n");
	printf("chcmpss <inputFile> <outputFile> : compress inputFile to outputFile\n");
	printf("chcmpss -p <inputFile> : print prefixTable of inputFile (but don't compress it)\n");
	printf("chcmpss -r <inputFile> : read prefixTable of compressed inputFile\n");
}

int parseArgument(int argc, char **argv)
{
	if (argc == 1) {
		printSyntax();
		return PA_ERROR;
	}
	if (argc ==2) {
		printSyntax();
		if (argv[1][0] == '-') if (argv[1][1] == 'h' && argv[1][2] == 0) return PA_SUCCESS;
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
	if (argv[1][1] == 'r') {
		readPrefixTable = true;
		inputFile = argv[2];
		return PA_SUCCESS;
	}
	printSyntax();
	return PA_ERROR;
}
