#ifndef _LIST_H
#define _LIST_H

#include <stdbool.h>

typedef struct _ch_ordered_list_node {
	void *value;
	struct _ch_ordered_list_node *previous;
	struct _ch_ordered_list_node *next;
} ch_list_node;

typedef struct _ch_ordered_list {
	unsigned len;
	ch_list_node *head;
	ch_list_node *tail;
	int (*compare)(void *, void *); 
	void (*destroyNode)(void *);
} ch_ordered_list;

// CONSTANTS
#define CH_LIST_SUCCESS 0
#define CH_LIST_ERROR -1

// Prototypes

ch_ordered_list *createOrderedList(int (*)(void *, void *), void (*)(void *));
void freeOrderedList(ch_ordered_list *);
int addNode(ch_ordered_list *, void *);
int removeFirstNode(ch_ordered_list *, bool);

#endif //LIST_H