#ifndef _CMPSSBUFFER_H
#define _CMPSSBUFFER_H

#include "htree.h"

#define CMPSSBUFFER_ERROR 1
#define CMPSSBUFFER_SUCCESS 0
#define CMPSSBUFFER_OUTPUT_ERROR 2
#define CMPSSBUFFER_WRITE_ERROR 3
#define CMPSSBUFFER_FREQ_TABLE_INTERNAL_ERROR 4


int prepareBitBuffer(void);
int saveNode(htree_node *);
int closeBitBuffer(void);
int finishSavingPrefixTable(void);
int saveFrequency(char *);

#endif // _CMPSSBUFFER_H
