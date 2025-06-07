/***
* Build Your Own wc Tool
*
* This challenge is to build your own version of the Unix command line tool wc!
*
***/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <wchar.h>

#define SUCCESS 0
#define PARSE_ARGUMENT_ERROR 1
#define COUNT_BYTES_ERROR -1
#define COUNT_LINES_WORDS_ERROR -1

int countBytes = 0;
int countLines = 0;
int countWords = 0;
int countChars = 0;
int noOptions = 0;
int readStdin = 0;
char *fileName = NULL;

int parseArgument(int argc, char **argv)
{
	if (argc == 1) {
		noOptions = 1;
		readStdin = 1;
		return SUCCESS;
	}
	
	if (argv[1][0] == '-') {
			int len = strlen(argv[1]);
			
			if (len != 2) {
parse_error:
					printf("Option unrecognized\n");
					return PARSE_ARGUMENT_ERROR;
			}
			switch (argv[1][1]) {
				case 'c': 
					countBytes = 1;
					break;
				case 'l':
					countLines = 1;
					break;
				case 'w':
					countWords = 1;
					break;
				case 'm':
					countChars = 1;
					break;	
				default:
					goto parse_error;				
			}
	}
	else {
		fileName = argv[1];
		noOptions = 1;
	}
	if (argc == 2) {
		if (!fileName) readStdin = 1;
		return SUCCESS;
	}
	if (fileName) {
		printf("Command syntax error\n");
		return PARSE_ARGUMENT_ERROR;
	}
	fileName = argv[2];
	if (argc > 3) {
		printf("Arguments not recognized\n");
		return PARSE_ARGUMENT_ERROR;
	}
	return SUCCESS;
}

FILE *openFile(char *mode)
{
	if (!fileName && !readStdin) {
		printf("File name not defined\n");
		return NULL;
	}
	if (!fileName && readStdin) fileName = "";
	
	FILE *f	= (readStdin) ? stdin : fopen(fileName, mode);
	
	if (!f) {
		printf("Could not open file %s\n", fileName);
		return NULL;
	}
	return f;
}

long fileBytes(void)
{
	FILE *f = openFile("rb");
	long count = 0;
	
	if (!f) return COUNT_BYTES_ERROR;
	if (fseek(f, 0, SEEK_END) != 0) {
		printf("Could not read file %s\n", fileName);
		count = COUNT_BYTES_ERROR;
		goto countBytes_closeFile;
	}
	count = ftell(f);
	if (count == -1) {
		printf("Could not calculate file size of %s\n", fileName);
		count = COUNT_BYTES_ERROR;
	}
	
countBytes_closeFile:
	fclose(f);
	return count;
}

void countFileBytes(void)
{
	if (readStdin) {
		printf("Option not implemented for pipe\n");
		return;
	}
	
	long countBytes = fileBytes();
	
	if (countBytes == COUNT_BYTES_ERROR) return;
	printf(" %ld %s\n", countBytes, fileName);
}

int countLinesWords(long *lines, long *words)
{
	FILE *f = openFile("r");
	
	if (!f) return COUNT_LINES_WORDS_ERROR;
	
	char c,
	     prev = '\n';
	int inWord = 0;
		 
	while((c = fgetc(f)) != EOF) {
		if (c == '\n') *lines = *lines + 1;
		prev = c;
		if (isspace(c)) {
			if (inWord) {
				inWord = 0;
				*words = *words + 1;
			}
		}
		else inWord = 1;
	}
	if (inWord) *words = *words + 1;
	if (prev != '\n' && prev != EOF) *lines = *lines + 1;
	fclose(f);
	return SUCCESS;
}

void countFileLines(void)
{
	long lines = 0, 
	     words = 0;
		 
	
	if (countLinesWords (&lines, &words) == SUCCESS)
		printf (" %ld %s\n", lines, fileName);
}

void countFileWords()
{
	long lines = 0, 
	     words = 0;
		 
	
	if (countLinesWords (&lines, &words) == SUCCESS)
		printf (" %ld %s\n", words, fileName);
}

void countFileChars(void)
{
	if (setlocale(LC_ALL, "") == NULL)
        printf("Warning: could not set locale. Multi-byte decoding may fail.\n");
	
	FILE *f = openFile("r");
	
	if (!f) return;
	
	wint_t wc;
    long count = 0;
	
    
	while ((wc = fgetwc(f)) != WEOF) {
		count++;
	}
	
	printf (" %ld %s\n", count, fileName);
	fclose(f);
	return;
}

void countBytesLinesWords(void)
{
	long countBytes = 0;
	
	if (!readStdin)
		countBytes  = fileBytes();
	
	if (countBytes == COUNT_BYTES_ERROR) return;
	
	long countLines = 0,
	     countWords = 0;
	
	if (countLinesWords (&countLines, &countWords) != SUCCESS) return;
	if (!readStdin)	printf (" %ld %ld %ld %s\n", countLines, countWords, countBytes, fileName);
	else printf (" %ld %ld\n", countLines, countWords);
}

int main(int argc, char **argv)
{
	int ret = parseArgument(argc, argv);
	
	if (ret == PARSE_ARGUMENT_ERROR) return PARSE_ARGUMENT_ERROR;
	if (countBytes) countFileBytes();
	if (countLines) countFileLines();
	if (countWords) countFileWords();
	if (countChars) countFileChars();
	if (noOptions) countBytesLinesWords();
	return 0;
}