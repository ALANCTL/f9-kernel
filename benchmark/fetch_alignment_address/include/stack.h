#ifndef STACK_H_
#define STACK_H

typedef struct {
	int *elems;
	int top;
	int size;
} stack;

void create_stack (stack *s, int size);
void destory_stack (stack *s);
void push_stack (stack *s, int value);
int* pop_stack (stack *s);
void iteration_stack (stack *);

#endif
