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
#define IN_ARRAY 8
#define IN_OBJECT 9
#define OBJECT_VALID 10
#define OBJECT_INVALID 11
#define ARRAY_VALID 12
#define ARRAY_INVALID 13
#define ELEMENT_VALID 14
#define ELEMENT_INVALID 15
#define ELEMENT_VALID_CHAR_ALREADY_READ 16

// Macro

#define CHAR_READ(ERROR) c = fgetc(f); \
	if (c == EOF) goto ERROR; \
	else if (c == '\n') line++;
	
// Prototypes

int parseObject(FILE *);
int parseArray(FILE *);

// globals

int line = 1;

int parseString(FILE *f) 
{
	char c;
		
ps_st0:
    CHAR_READ (ps_end)
	if ((c == '\t') || (c == '\n') || (c == '\r')) return STRING_INVALID;
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
	bool onlyZero = false,
	     numAfterE = false;
	
	if (*ch == '0') onlyZero = true;
	
	char c;
	int numberLen = 0;
	
pn_st0:
	CHAR_READ (pn_end);
	numberLen++;
	if (isdigit(c)) {
		if (c == '0' && bMinus) return onlyZero = true;
		bMinus = false;
		goto pn_st0;
	}
	if (c == '.') {
		if (bMinus) return NUMBER_INVALID;
		goto pn_st1;
	}
	if (c == 'E' || c == 'e') goto pn_st3;
pn_st0_1:
	*ch = c;
	if (onlyZero && numberLen == 1) return NUMBER_VALID;
	if (bMinus) return NUMBER_INVALID;
	if (onlyZero) return NUMBER_INVALID;
	return NUMBER_VALID;
	
pn_st1:
	onlyZero = false;
    CHAR_READ (pn_end);
    if (!isdigit(c)) return NUMBER_INVALID;
	numberLen++;
   
pn_st2:
	CHAR_READ (pn_end);
	if (c == 'E' || c == 'e') goto pn_st3;
	if (isdigit(c)) {
		numberLen++;
		goto pn_st2;
	}
	goto pn_st0_1;
	
pn_st3:
	numberLen++;
	CHAR_READ (pn_end);
	if (c != '+' && c != '-' && !isdigit(c)) goto pn_end;
	numberLen++;
	if (isdigit(c)) numAfterE = true;

pn_st4:	
	CHAR_READ (pn_end);
	if (isdigit(c)) {
		numAfterE = true;
		numberLen++;
		goto pn_st4;
	}
	if (!numAfterE) return NUMBER_INVALID;
	//printf("Aqui 2\n");
	goto pn_st0_1;

pn_end:
	return NUMBER_INVALID;
	
}

int parseElement(FILE *f, char *pc, bool usePointer)
{
	char c;
		
	if (usePointer) {
		c = *pc;
		goto pe_st1;
	}
	
pe_st0:
	CHAR_READ (pe_end);
pe_st1:
	if (isspace(c)) goto pe_st0;
		
	if (c == 't') {
		if (parseToken(f, "rue") == TOKEN_INVALID) return ELEMENT_INVALID;
		return ELEMENT_VALID;
	}
	if (c == 'n') {
		if (parseToken(f, "ull") == TOKEN_INVALID) return ELEMENT_INVALID;
		return ELEMENT_VALID;
	}
	if (c == 'f') {
		if (parseToken(f, "alse") == TOKEN_INVALID) return ELEMENT_INVALID;
		return ELEMENT_VALID;	
	}
	if (c == '"') {
		if (parseString(f) == STRING_INVALID) return ELEMENT_INVALID;
		return ELEMENT_VALID;
	}
	if (c == '-') {
		if (parseNumber(f, true, &c) == NUMBER_INVALID) goto pe_end;
		*pc = c;
		return ELEMENT_VALID_CHAR_ALREADY_READ;
	}
	if (isdigit(c)) {
	    if (parseNumber(f, false, &c) == NUMBER_INVALID) goto pe_end;
		*pc = c;
		return ELEMENT_VALID_CHAR_ALREADY_READ;
	}
	if (c == '[') {
		if (parseArray(f) == ARRAY_INVALID) return ELEMENT_INVALID;
		return ELEMENT_VALID;
	}
	if (c == '{') {
		if (parseObject(f) == OBJECT_INVALID) return ELEMENT_INVALID;
		return ELEMENT_VALID;
	}
	
pe_end:
	return ELEMENT_INVALID;
	
}

int parseObject(FILE *f)
{
	char c;
	int ret;
	
po_st0:
	CHAR_READ (po_end)
	if (isspace(c)) goto po_st0;
	if (c == '"') goto po_st1;
	if (c != '}') return OBJECT_INVALID;
	return OBJECT_VALID;
	
po_st1: 
	if (parseString(f) == STRING_INVALID) return OBJECT_INVALID;
	
po_st2:
	CHAR_READ (po_end)
	if (isspace(c)) goto po_st2;
	if (c != ':') return OBJECT_INVALID;
	if ((ret = parseElement(f, &c, false)) == ELEMENT_INVALID) return OBJECT_INVALID;
	if (ret == ELEMENT_VALID_CHAR_ALREADY_READ) goto po_st4;
	
po_st3:
	CHAR_READ (po_end)
	
po_st4:
	if (isspace(c)) goto po_st3;
	if (c == ',') goto po_st5;
	if (c != '}') return OBJECT_INVALID;
	return OBJECT_VALID;
	
po_st5:
	CHAR_READ (po_end);
	if (isspace(c)) goto po_st5;
	if (c == '"') goto po_st1;
	return OBJECT_INVALID;
	
	
po_end:
	return OBJECT_INVALID;
}

int parseArray(FILE *f)
{
	char c;
	int ret;
	
pa_st0:
	CHAR_READ (pa_end);
	if (isspace(c)) goto pa_st0;
	if (c == ']') return ARRAY_VALID;
pa_st1:
	if ((ret = parseElement(f, &c, true)) == ELEMENT_INVALID) return ARRAY_INVALID;
	if (ret == ELEMENT_VALID_CHAR_ALREADY_READ) goto pa_st3;
	
pa_st2:
	CHAR_READ (pa_end);
pa_st3:
	if (isspace(c)) goto pa_st2;
	if (c == ']') return ARRAY_VALID;
	if (c != ',') return ARRAY_INVALID;
	
pa_st4:
	CHAR_READ (pa_end);
	if (isspace(c)) goto pa_st4;
	goto pa_st1;

pa_end:
	return ARRAY_INVALID;
}

int parseJson(char *fileName)
{
	FILE *f = fopen(fileName, "r");
	
	if (!f) {
		printf("Could not open file %s\n", fileName);
		return JSON_INVALID;
	}
	
	bool valid = false;
	char c;
	
pj_st0:	
	CHAR_READ (pj_end)
	if (isspace(c)) goto pj_st0;
	if (c == '[') goto pj_st1;
	if (c != '{') goto pj_end;
	if (parseObject(f) == OBJECT_INVALID) goto pj_end;
	valid = true;
	goto pj_st2;
	
pj_st1:	
    if (parseArray(f) == ARRAY_INVALID) goto pj_end;
	valid = true;
	
pj_st2:
	CHAR_READ (pj_end);
	if (!isspace(c)) {
		valid = false;
		goto pj_end;
	}
	goto pj_st2;
	
pj_end:
	fclose(f);
	if (valid) {
		printf("JSON file %s is valid\n", fileName);
		return JSON_VALID;
	}
	printf("JSON file %s is INvalid (line: %d)\n", fileName, line);
	return JSON_INVALID;
	

}

int main(int argc, char **argv)
{
	if (parseArgument(argc, argv) == PA_ERROR)
		return JSON_INVALID;
	return parseJson(argv[1]);
}