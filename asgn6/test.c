#include "pq.h"
#include "node.h"
#include "code.h"
#include "io.h"
#include "stack.h"

#include <stdlib.h>
#include <stdio.h>

// Tests PriorityQueue functions and usability.
void pq_test(void) {
    printf("Priority Queue test:\n");
    PriorityQueue *q = pq_create(10);
    Node **nodes = (Node **) malloc(10 * sizeof(Node *));
    for (int i = 0; i < 10; i++) {
        Node *n = node_create('a' + i, random() % (i + 10));
        enqueue(q, n);
        nodes[i] = n;
        pq_print(q);
    }
    Node *n;
    for (int i = 0; i < 10; i++) {
        dequeue(q, &n);
        node_print(n);
        node_delete(&n);
    }
    pq_delete(&q);
    free(nodes);
    return;
}

// Tests Code functions and usability.
void code_test(void) {
    printf("Code test:\n");
    Code c = code_init();
    code_push_bit(&c, 1);
    code_push_bit(&c, 0);
    code_push_bit(&c, 1);
    code_print(&c);
    uint8_t x;
    code_pop_bit(&c, &x);
    code_print(&c);
    printf("%d\n", x);
    code_push_bit(&c, 0);
    code_print(&c);
    return;
}

// Tests IO functions and usability.
void io_test(void) {
    printf("IO test (enter input then CTRL+D):\n");
    uint8_t *buf = calloc(100, sizeof(uint8_t));
    read_bytes(fileno(stdin), buf, 100);
    write_bytes(fileno(stdout), buf, 100);

    for (int b = 0; b < BLOCK + 1; b++) {
        Code c = code_init();
        for (int i = 0; i < (b == BLOCK ? 5 : 6); i++) {
            code_push_bit(&c, 0);
        }
        code_push_bit(&c, 1);
        for (int i = 0; i < 1; i++) {
            code_push_bit(&c, 0);
        }
        write_code(fileno(stdout), &c); // should print out the first 4096
    }
    printf("\n");
    flush_codes(fileno(
        stdout)); // print the space from the 4097th byte (should be a space and not a "`" (96 ascii, 64 + 32))
    printf("\n");
    flush_codes(fileno(stdout)); // print nothing
    free(buf);
    return;
}

// Tests Stack functions and usability.
void stack_test(void) {
    Stack *stack = stack_create(10);
    for (int i = 0; i < 11; i++) { // 11 to test full stack
        stack_push(stack, node_create('a' + i, i));
    }
    stack_print(stack);
    Node *n;
    for (int i = 0; i < 11; i++) {
        if (!stack_pop(stack, &n)) {
            printf("Stack is empty\n");
            continue;
        }
        node_print(n);
        node_delete(&n);
    }
    stack_delete(&stack);
    return;
}

int main(void) {
    pq_test();
    code_test();
    io_test();
    stack_test();
    return 0;
}
