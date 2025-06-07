/***
* Challenge: Build Your Own JSON Parser
***/

#include "parse_argument.h"
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#define JSON_VALID 0
#define JSON_INVALID 1
#define STRING_VALID 2
#define STRING_INVALID 3

// Macro

#define CHAR_READ(ERROR) c = fgetc(f); \
	if (c == EOF) goto ERROR;

int parseString(FILE *f) 
{
	char c;
	
ps_st0:
    CHAR_READ (ps_end)
	if (c == '\\') goto ps_st1;
	if (c == '"') return STRING_VALID;
	goto ps_st0;
	
ps_st1:
	CHAR_READ (ps_end)
	switch (c) {
		case '"':
		case '\\':
		case '/':
		case 'b':
		case 'f':
		case 'n':
		case 'r':
		case 't':
			goto ps_st0;
		case 'u': 
			break;
		default:
			goto ps_end;
	}
	for (int i = 0; i < 4; i++) {
		CHAR_READ (ps_end);
		if (!isxdigit(c)) goto ps_end;
	}
	goto ps_st0;
	
ps_end:
	return STRING_INVALID;
}

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
	CHAR_READ (pj_end)
	if (isspace(c)) goto pj_st0;
	if (c != '{') goto pj_end;
	
pj_st1:
	CHAR_READ (pj_end)
	if (isspace(c)) goto pj_st1;
	if (c == '"') goto pj_st3;
	if ( c != '}') goto pj_end;
	valid = true;
	
pj_st2:  
	CHAR_READ (pj_end)
	if (isspace(c)) goto pj_st2;
	valid = false;
	goto pj_end;
	
pj_st3: 	
	if (parseString(f) == STRING_INVALID) goto pj_end;
	CHAR_READ (pj_end)
	if (isspace(c)) goto pj_st3;
	if (c != ':') goto pj_end;
	
pj_st4:
	CHAR_READ (pj_end);
	if (isspace(c)) goto pj_st4;
	if (c != '"') goto pj_end;
	
	if (parseString(f) == STRING_INVALID) goto pj_end;
	
pj_st5: 
	CHAR_READ (pj_end);
	if (isspace(c)) goto pj_st5;
	if (c == ',') goto pj_st6;
	if ( c != '}') goto pj_end;
	valid = true;
	goto pj_st2;
	
pj_st6:
	CHAR_READ (pj_end);
	if (isspace(c)) goto pj_st6;
	if (c == '"') goto pj_st3;

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