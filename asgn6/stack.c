#include "stack.h"
#include "node.h"

#include <stdlib.h>
#include <stdio.h>

struct Stack {
    uint32_t max_size;
    uint32_t size;
    Node **values;
};

// Creates a stack with
// the specified capacity.
// Returns: a pointer to the created Stack
//
// capacity: the size of the Stack
Stack *stack_create(uint32_t capacity) {
    Node **values = (Node **) malloc(capacity * sizeof(Node *));
    Stack *stack = (Stack *) malloc(sizeof(Stack));
    stack->max_size = capacity;
    stack->size = 0;
    stack->values = values;
    return stack;
}

// Deletes the Stack and frees the
// associated memory.
//
// s: a pointer to the Stack's address (to set to NULL)
void stack_delete(Stack **s) {
    free((*s)->values);
    free(*s);
    s = NULL;
    return;
}

// Returns whether or not the Stack is empty.
//
// s: the Stack pointer to check
bool stack_empty(Stack *s) {
    return s->size == 0;
}

// Returns whether or not the Stack is full.
//
// s: the Stack pointer to check
bool stack_full(Stack *s) {
    return s->size == s->max_size;
}

// Returns the size of the Stack.
//
// s: the Stack pointer to check
uint32_t stack_size(Stack *s) {
    return s->size;
}

// Puts a Node pointer at the top of the Stack.
// Returns: whether or not the pointer was pushed
//
// s: the Stack pointer to push to
// n: the Node pointer to push onto the Stack
bool stack_push(Stack *s, Node *n) {
    if (stack_full(s)) {
        return false;
    }
    s->values[s->size++] = n;
    return true;
}

// Pops the topmost value from the Stack
// into the location of the Node pointer.
// Returns: whether or not the pointer was popped
//
// s: the Stack pointer to pop from
// n: the Node address pointer to set to the popped Node pointer
bool stack_pop(Stack *s, Node **n) {
    if (stack_empty(s)) {
        return false;
    }
    *n = s->values[--s->size];
    return true;
}

// Prints the Stack for debugging.
//
// s: the Stack pointer to print
void stack_print(Stack *s) {
    printf("---Stack Start---\n");
    for (uint32_t i = 0; i < s->size; i++) {
        node_print(s->values[i]);
    }
    printf("----Stack End----\n");
    return;
}
