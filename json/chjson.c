/***
* Challenge: Build Your Own JSON Parser
***/

#include "parse_argument.h"
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#define JSON_VALID 0
#define JSON_INVALID 1
#define STRING_VALID 2
#define STRING_INVALID 3
#define TOKEN_VALID 4
#define TOKEN_INVALID 5
#define NUMBER_VALID 6
#define NUMBER_INVALID 7

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

int parseToken(FILE *f, char *token)
{
	int len = strlen(token);
	char c;
	
	for (int i = 0; i < len; i++) {
		CHAR_READ (parse_token_end);
		if (c != token[i]) return TOKEN_INVALID;
	}
	return TOKEN_VALID;
	
parse_token_end:
	return TOKEN_INVALID;
}

int parseNumber(FILE *f, bool bMinus, char *ch)
{
	char c;
	
pn_st0:
	CHAR_READ (pn_end);
	if (isdigit(c)) {
			bMinus = false;
			goto pn_st0;
	}
	if (c == '.') goto pn_st1;
	if (c == 'E' || c == 'e') goto pn_st3;
pn_st0_1:
	*ch = c;
	if (bMinus) return NUMBER_INVALID;
	return NUMBER_VALID;
	
pn_st1:
   CHAR_READ (pn_end);
   if (!isdigit(c)) return NUMBER_INVALID;
   
pn_st2:
	CHAR_READ (pn_end);
	if (c == 'E' || c == 'e') goto pn_st3;
	if (isdigit(c)) goto pn_st2;
	goto pn_st0_1;
	
pn_st3:
	CHAR_READ (pn_end);
	if (c != '+' && c != '-' && !isdigit(c)) goto pn_end;

pn_st4:	
	CHAR_READ (pn_end);
	if (isdigit(c)) goto pn_st4;
	goto pn_st0_1;

pn_end:
	return NUMBER_INVALID;
	
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
pj_st4:
	CHAR_READ (pj_end)
	if (isspace(c)) goto pj_st4;
	if (c != ':') goto pj_end;
	
pj_st5:
	CHAR_READ (pj_end);
	if (isspace(c)) goto pj_st5;
	if (c == 't') {
		if (parseToken(f, "rue") == TOKEN_INVALID) goto pj_end;
		goto pj_st6;
	}
	if (c == 'n') {
		if (parseToken(f, "ull") == TOKEN_INVALID) goto pj_end;
		goto pj_st6;	
	}
	if (c == 'f') {
		if (parseToken(f, "alse") == TOKEN_INVALID) goto pj_end;
		goto pj_st6;	
	}
	if (c == '-') {
		if (parseNumber(f, true, &c) == NUMBER_INVALID) goto pj_end;
		goto pj_st6_1;
	}
	if (isdigit(c)) {
	    if (parseNumber(f, false, &c) == NUMBER_INVALID) goto pj_end;
		goto pj_st6_1;	
	}
	if (c != '"') goto pj_end;
	if (parseString(f) == STRING_INVALID) goto pj_end;
	
pj_st6: 
	CHAR_READ (pj_end);
pj_st6_1:
	if (isspace(c)) goto pj_st6;
	if (c == ',') goto pj_st7;
	if ( c != '}') goto pj_end;
	valid = true;
	goto pj_st2;
	
pj_st7:
    valid = false;
	CHAR_READ (pj_end);
	if (isspace(c)) goto pj_st7;
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