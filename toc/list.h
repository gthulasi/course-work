#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
	int data;
	struct node *next;
} list;

struct node *new_node(int val)
{
	struct node *node = calloc(1, sizeof(list));
	if (node != NULL) {
		node->data = val;
		node->next = NULL;
	}
	return node;
}

list * insert(list *l, int val)
{
	struct node *node = new_node(val);
	if (node)
		node->next = l;
	return node;
}

struct node * search(list *l, int val)
{
	struct node *current = l;

	while (current != NULL) {
		if (current->data == val)
			break;
		current = current->next;
	}
	return current;
}


list * delete_list(list *l)
{
	struct node *current = l;

	while (current != NULL) {
		list *tmp = current;
		current = current->next;
		free(tmp);
	}

	return NULL;
}

void print_list(list *l)
{
	struct node *current = l;

	while (current != NULL) {
		printf("%d ", current->data);
		current = current->next;
	}
	printf("\n");
}

