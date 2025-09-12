#include "parse_argument.h"
#include <stdio.h>
#include <stdbool.h>

extern char *inputFile;
extern char *outputFile;
extern bool printPrefixTable;
extern bool readPrefixTable;
extern bool decompressFile;


void printSyntax(void)
{
	printf("Command syntax:\n");
	printf("chcmpss -h: print this message\n");
	printf("chcmpss <inputFile> <outputFile> : compress inputFile to outputFile\n");
	printf("chcmpss -d <inputFile> <outputFile> : decompress inputFile to outputFile\n");
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
	if (argc != 3 && argc != 4) {
		printSyntax();
		return PA_ERROR;
	}
	if (argv[1][0] != '-' && argc == 3) {
		outputFile = argv[2];
		inputFile = argv[1];
		return PA_SUCCESS;
	}
	if (argv[1][0] != '-') goto error;
	if (argv[1][1] == 'p' && argc == 3) {
		printPrefixTable = true;
		inputFile = argv[2];
		return PA_SUCCESS;
	}
	if (argv[1][1] == 'r' && argc == 3) {
		readPrefixTable = true;
		inputFile = argv[2];
		return PA_SUCCESS;
	}
	//printf("Aqui\n");
	if (argv[1][1] == 'd' && argc == 4) {
		decompressFile = true;
		outputFile = argv[3];
		inputFile = argv[2];
		return PA_SUCCESS;
	}
error:
	printSyntax();
	return PA_ERROR;
}
