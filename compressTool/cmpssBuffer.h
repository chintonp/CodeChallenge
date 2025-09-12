#ifndef _CMPSSBUFFER_H
#define _CMPSSBUFFER_H

#include "htree.h"
#include <stdio.h>

#define CMPSSBUFFER_ERROR 1
#define CMPSSBUFFER_SUCCESS 0
#define CMPSSBUFFER_OUTPUT_ERROR 2
#define CMPSSBUFFER_WRITE_ERROR 3
#define CMPSSBUFFER_FREQ_TABLE_INTERNAL_ERROR 4
#define CMPSSBUFFER_READ_ERROR 5
#define CMPSSBUFFER_READ_EOF 6
#define CMPSSBUFFER_CORRUPTED_FILE 7
#define CMPSSBUFFER_NOT_ENOYGH_MEMORY 8


int prepareBitBuffer(void);
int saveNode(htree_node *);
int closeBitBuffer(void);
int finishSavingPrefixTable(void);
int saveFrequency(char *);
void prepareExtractPrefixTable(void);
int read8(FILE *, uint8_t *);
int readFreqTable(FILE *);
int readLetter(FILE *, uint8_t *);

#endif // _CMPSSBUFFER_H
