#include "../../include/stack.h"
#include <stdlib.h>
#include <stdio.h>

void create_stack (stack *s, int size)
{
	s->top = 0;
	s->size = size;
	s->elems = (int *) malloc (s->size * sizeof (int));
}

void destory_stack (stack *s)
{
	free (s->elems);
}

void push_stack (stack *s, int value)
{
	if (s->top == s->size) {
		s->size <<= 1;

		s->elems = realloc (s->elems, s->size * sizeof (int));
	}

	s->elems[s->top] = value;
	s->top++;
}

int* pop_stack (stack *s)
{
	s->top--;

	s->size = s->top;

	s->elems = realloc (s->elems, s->top * sizeof (int));

	return s->elems;
}

void iteration_stack (stack *s)
{
	for (int i = 0; i < s->top; ++i) {
		printf ("(%d, %d)\n", i, s->elems[i]);
	}
}
