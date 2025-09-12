#ifndef _HTREE_H
#define _HTREE_H

#include "list.h"
#include <stdint.h>

// Constants
#define HT_SUCCESS 0
#define HT_ERROR -1
#define PREFIX_CODE_MAX_LEN 65

typedef struct _htree_node {
	uint8_t letter;
	unsigned frequency;
	char prefixCode[PREFIX_CODE_MAX_LEN];
	struct _htree_node *leftNode;
	struct _htree_node *rightNode;
} htree_node;

// Prototypes

ch_ordered_list *createHuffmanList (htree_node [], unsigned);
htree_node *createHuffmanNode(char, unsigned);
int createBinaryTree(ch_ordered_list *);
int createPrefixTable(ch_ordered_list *);
int createPrefixNode(uint8_t, char *);
void destroyPrefixTable(void);

#endif // _H_TREE_H
