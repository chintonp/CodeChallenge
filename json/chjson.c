/***
* Challenge: Build Your Own JSON Parser
***/

#include "parse_argument.h"
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#define JSON_VALID 0
#define JSON_INVALID 1

// Macro

#define CHAR_READ c = fgetc(f); \
	if (c == EOF) goto pj_end;

int parseJson(char *fileName)
{
	FILE *f = fopen(fileName, "r");
	
	if (!f) {
		printf("Could not open file %s\n", fileName);
		return JSON_INVALID;
	}
	
	char c;
	bool valid = false;

pj_st0:	
	CHAR_READ
	if (isspace(c)) goto pj_st0;
	if (c != '{') goto pj_end;
	
pj_st1:
	CHAR_READ
	if (isspace(c)) goto pj_st1;
	if ( c != '}') goto pj_end;
	valid = true;
	
pj_st2:
	CHAR_READ
	if (isspace(c)) goto pj_st2;
	valid = false;

pj_end:
	fclose(f);
	if (valid) {
		printf("JSON file %s is valid\n", fileName);
		return JSON_VALID;
	}
	printf("JSON file %s is INvalid\n", fileName);
	return JSON_INVALID;
}


int main(int argc, char **argv)
{
	if (parseArgument(argc, argv) == PA_ERROR)
		return JSON_INVALID;
	return parseJson(argv[1]);
}