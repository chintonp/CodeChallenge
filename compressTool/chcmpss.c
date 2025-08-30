#include "parse_argument.h"
#include <stdio.h>
#include "htree.h"
#include "cmpssBuffer.h"

#define SUCCESS 0
#define ERROR -1
#define FREQ_TABLE_LEN 256

char *inputFile = NULL;
char *outputFile = NULL;
bool printPrefixTable = false;
htree_node freqTable[FREQ_TABLE_LEN];

int savePrefixTable(ch_ordered_list *huffmanList)
{
	if (!huffmanList) return ERROR;
	if (!outputFile) return ERROR;

	int ret = prepareBitBuffer();

	if (ret == CMPSSBUFFER_OUTPUT_ERROR) {
		printf("Could not open the output file %s\n", outputFile);
		return ERROR;
	}

	if (huffmanList -> head == NULL) {
		printf("Internal error (01)\n");
		return ERROR;
	}

	htree_node *ht_node = (htree_node *) huffmanList -> head -> value;

	if (ht_node == NULL) {
		printf("Internal error (02)\n");
		return ERROR;
	}

	ret = saveNode(ht_node);
	if (ret != CMPSSBUFFER_SUCCESS) {
		printf("Writing error\n");
		return ERROR;
	}
	ret = finishSavingPrefixTable();
	if (ret != CMPSSBUFFER_SUCCESS) {
		printf("Writing error\n");
		return ERROR;
	}

	return SUCCESS;
}

int calculatePrefixTable (FILE *f)
{
	for (int i = 0; i < FREQ_TABLE_LEN; i++) {
		freqTable[i].letter = (uint8_t) i;
		freqTable[i].frequency = 0;
		freqTable[i].prefixCode[0] = 0;
		freqTable[i].leftNode = NULL;
		freqTable[i].rightNode = NULL;
	}
	
//#ifndef _DEBUG
	
	int c = fgetc(f);
	
	while (c != EOF) {
		/*if (c < 256)*/ freqTable[c].frequency++;
		c = fgetc(f);
	}
	
//#endif

#ifdef _DEBUG

	// Define table for tests
/*
	freqTable['A'].frequency = 1;
	freqTable['B'].frequency = 2;
	freqTable['C'].frequency = 9;
	freqTable['D'].frequency = 5;
	freqTable['E'].frequency = 11;
	freqTable['F'].frequency = 1;
	freqTable['G'].frequency = 3;
	freqTable['H'].frequency = 2;
//*/
//#ifdef _DEBUG
	printf("\nCharacter frequencies:\n");
	
	for (int i = 0; i < FREQ_TABLE_LEN; i++) {
		if (freqTable[i].frequency == 0) continue;
		if (i >= 32 && i <= 126) {
                printf("'%c' : %d\n", i, freqTable[i].frequency);
        } 
		else {
			// For non-printable characters, print their ASCII value
            printf("ASCII %d : %d\n", i, freqTable[i].frequency);
        }
	}
#endif //_DEBUG

	ch_ordered_list *huffmanList = createHuffmanList(freqTable, FREQ_TABLE_LEN);
	
	if (!huffmanList) return ERROR;
	
	int ret = createBinaryTree(huffmanList);

	if (ret == HT_ERROR) return ERROR;
	ret = createPrefixTable(huffmanList);
	if (ret == HT_ERROR) {
		freeOrderedList(huffmanList);
		return ERROR;
	}
	if (printPrefixTable) {
		freeOrderedList(huffmanList);
		return SUCCESS;
	}
	ret = savePrefixTable(huffmanList);

	freeOrderedList(huffmanList);
	return ret;
}

int compressFile(FILE *f)
{
	rewind(f);
	//printf("Aqui: cf\n");

	int c = fgetc(f);

	while (c != EOF) {
		int ret = saveFrequency(freqTable[c].prefixCode);

		if (ret != CMPSSBUFFER_SUCCESS) return ERROR;
		c = fgetc(f);
	}
	return SUCCESS;
}


int main(int argc, char **argv)
{
	if (parseArgument(argc, argv) == PA_ERROR) return ERROR;
	if (!inputFile) return ERROR;
	
	FILE *f = fopen(inputFile, "r");
	
	if (f == NULL) {
		printf ("Could not open file %s\n", inputFile);
		return ERROR;
	}
	
	int ret = calculatePrefixTable(f);

	if (ret == ERROR) {
		goto end;
	}
	if (printPrefixTable) goto end;
	ret = compressFile(f);
	if (ret == SUCCESS) ret = closeBitBuffer();

end:
	fclose(f);
	return ret;
}
