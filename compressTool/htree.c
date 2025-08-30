#include "htree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "cmpssBuffer.h"

extern bool printPrefixTable;

int huffmanCompare (void *node1, void *node2)
{
	htree_node *hn1 = (htree_node *) node1,
		*hn2 = (htree_node *) node2;
		
	return (hn1 -> frequency - hn2 -> frequency);
}

void destroyHuffmanNode(void *node)
{
	if (!node) return;
	
	htree_node *ht_node = (htree_node *) node;
	
	if (ht_node -> leftNode) destroyHuffmanNode (ht_node -> leftNode);
	if (ht_node -> rightNode) destroyHuffmanNode (ht_node -> rightNode);
	if (ht_node -> leftNode || ht_node -> rightNode) free (node);
}

htree_node *createHuffmanNode(char c, unsigned f)
{
	htree_node *node = (htree_node *)malloc(sizeof(htree_node));
	
	if (!node) return NULL;
	
	node -> letter = c;
	node -> frequency = f;
	node -> prefixCode[0] = 0;
	node -> leftNode = NULL;
	node -> rightNode = NULL;
	
	return node;
}

ch_ordered_list *createHuffmanList (htree_node freqTable[], unsigned len)
{
	ch_ordered_list *list = createOrderedList(huffmanCompare, NULL);
	
	if (!list) return NULL;
	
	for (unsigned i = 0; i < len; i++) {
		if (freqTable[i].frequency == 0) continue;
		
		int ret = addNode(list, &freqTable[i]);
		
		if (ret == CH_LIST_ERROR) goto error;
	}
	return list;
	
error:
	freeOrderedList(list);
	return NULL;
}

#ifdef _DEBUG

void printNode(htree_node *ht_n, int level)
{
	if (!ht_n) {
		printf("\n");
		return;
	}
	
	char c = ht_n -> letter;
	
	if (!c) c = ' ';
	printf("(%c) Freq: %u (%d)\n", c, ht_n -> frequency, level);
	printf("L ");
	printNode(ht_n -> leftNode, level + 1);
	printf("R ");
	printNode(ht_n -> rightNode, level + 1);
}

#endif // _DEBUG

int createBinaryTree (ch_ordered_list *list)
{
	if (list == NULL) return HT_ERROR;
	
	while (list -> len != 1) {
		ch_list_node *node1 = list -> head,
			*node2 = node1 -> next;

#ifdef _DEBUG
		if (!node1) printf ("Node1 NULL\n");
		if (!node2) printf ("Node2 NULL\n");
#endif // _DEBUG

		htree_node *ht_node1 = (htree_node *) node1 -> value,
			*ht_node2 = (htree_node *) node2 -> value;
			
		unsigned addedFreq = ht_node1 -> frequency + ht_node2 -> frequency;
		
		htree_node *new_node = createHuffmanNode(0, addedFreq);
		
		if (!new_node) return HT_ERROR;
		
		removeFirstNode(list, false);
		removeFirstNode(list, false);
		addNode(list, new_node);
		new_node -> leftNode = ht_node1;
		new_node -> rightNode = ht_node2;			
	}
	
#ifdef _DEBUG
//	htree_node *ht_n = (htree_node *) list -> head -> value;
//	printNode(ht_n, 0);

#endif // _DEBUG	
	return HT_SUCCESS;
}

int calculatePrefixCodeNode(htree_node *node, char *prefixCode, int i_code)
{
	if (!node) return HT_ERROR;

	int ret = HT_SUCCESS;

	if (node -> leftNode) {
		//printf("Left\n");
		prefixCode[i_code] = '0';
		ret = calculatePrefixCodeNode(node -> leftNode, prefixCode, i_code + 1);
 	}
 	if (node -> rightNode && ret == HT_SUCCESS) {
		//printf("Right\n");
		prefixCode[i_code] = '1';
		ret = calculatePrefixCodeNode(node -> rightNode, prefixCode, i_code + 1);
	}
	if (!node -> leftNode && !node -> rightNode) {
		prefixCode[i_code] = 0;
		strcpy(node -> prefixCode, prefixCode);
		if (printPrefixTable) {
			if (node -> letter >= 32 && node ->letter <= 126) {
				printf("'%c' : %s\n", node -> letter, prefixCode);
			}
			else {
				// For non-printable characters, print their ASCII value
				printf("ASCII %d : %s\n", node -> letter, prefixCode);
			}
		}
	}
	return ret;
}

int createPrefixTable(ch_ordered_list *list)
{
	if (list == NULL) return HT_ERROR;

	char prefixCode [PREFIX_CODE_MAX_LEN];
	htree_node *node = (htree_node *) list -> head -> value;

	int ret = calculatePrefixCodeNode(node, prefixCode, 0);

//#ifdef _DEBUG
	//if (ret == HT_ERROR) printf("!!!!! calcultePrefixCodeNode ERROR\n");
	//else printf(">>>>> calcultePrefixCodeNode SUCCESS\n");

//#endif // _DEBUG

	return ret;
}

