#include "list.h"
#include <stdlib.h>
#include <stdio.h>

ch_ordered_list *createOrderedList(int (*f_c)(void *, void *), void (*d_n)(void *)) {
		ch_ordered_list *list = (ch_ordered_list *)malloc(sizeof(ch_ordered_list));
		
		if (!list) return NULL;
		
		list->len = 0;
		list -> head = NULL;
		list -> tail = NULL;
		list -> compare = f_c;
		list -> destroyNode = d_n;
		
		return list;	
}

void freeOrderedList(ch_ordered_list *list)
{
	if (!list) return;
	
	ch_list_node *node = list -> head;

	//printf("List len: %d\n", list ->len);
	
	while (node != NULL) {
		//printf("free\n");
		ch_list_node *n = node -> next;
		
		if (node -> value && list -> destroyNode) list -> destroyNode(node -> value);
		free(node);
		node = n;
	}
	//printf("free list\n");
	free(list);
	return;
}

int addNode(ch_ordered_list *list, void *value)
{
	if (!list) return CH_LIST_ERROR;
	
	ch_list_node *node = (ch_list_node *)malloc(sizeof(ch_list_node));
	
	if (!node) return CH_LIST_ERROR;
	node -> value = value;
	node -> previous = NULL;
	node -> next =  NULL;
	
	if (list -> head == NULL) {
		list -> head = node;
		list -> tail = node;
		list -> len = 1;
		return CH_LIST_SUCCESS;
	}
	
	ch_list_node *node_toCompare = list -> head;
	
	while (node_toCompare) {
		int comp = list -> compare (node_toCompare -> value, node -> value);
		
		if (comp > 0) { // node_toCompare is bigger than node
			node -> previous = node_toCompare -> previous;
			if (node -> previous) {
				node -> previous -> next = node;
			}
			else {
				list -> head = node;
			}
			node -> next = node_toCompare;
			node_toCompare -> previous =  node;
			list -> len++;
			return CH_LIST_SUCCESS;
		}
		node_toCompare = node_toCompare -> next;
	}
	
	node_toCompare = list -> tail;
	node_toCompare -> next = node;
	list -> tail = node;
	node -> next = NULL;
	node -> previous = node_toCompare;
	list -> len++;
	return CH_LIST_SUCCESS;
}

int removeFirstNode(ch_ordered_list *list, bool deleteValue)
{
	if (!list) return CH_LIST_ERROR;
	
	ch_list_node *node = list -> head;
	
	if (!node) return CH_LIST_SUCCESS;

	list -> head = node -> next;
	if (node -> next) node -> next -> previous = NULL;
	list -> len--;
	if (list -> len == 0) {
		list -> head = NULL;
		list -> tail = NULL;
	}
	if (node -> value && list -> destroyNode && deleteValue) list -> destroyNode(node -> value);
	free(node);
	return CH_LIST_SUCCESS;
}
