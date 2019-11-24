#include <stdlib.h>
#include <stdint.h>

typedef struct {
	uint64_t * data;
	int size;
	int top;
} stack;

stack * new_stack(int size)
{
	stack *s = NULL;
	s = malloc(sizeof(stack));
	if (!s) return NULL;
	s->size = size;
	s->top = -1;
	s->data = (uint64_t *)malloc(sizeof(uint64_t *) * size);
	if (!s->data) {
		free(s);
		return NULL;
	}
	return s;
}

void free_stack(stack *s)
{
	if (s) {
		free(s->data);
		free(s);
	}
}

int is_full(stack *s)
{
	if (s->top == s->size - 1)
		return 1;
	return 0;
}

int is_empty(stack *s)
{
	if (s->top == -1)
		return 1;
	return 0;
}

int push(stack *s, uint64_t p)
{
	if (is_full(s))
		return 0;
	s->data[++(s->top)] = p;
	return 1;
}

uint64_t top(stack *s)
{
	if (is_empty(s))
		return 0;
	return s->data[s->top];
}

uint64_t pop(stack *s)
{
	if (is_empty(s))
		return 0;
	return s->data[s->top--];
}

int stack_size(stack *s)
{
	return (s->top + 1); 
}
