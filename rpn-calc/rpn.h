#ifndef RPN_H
#define RPN_H

#include <stddef.h>

typedef struct {
    double *items;
    size_t size;
    size_t cap;
} RpnStack;

RpnStack *rpn_stack_create(void);
void rpn_stack_free(RpnStack *stack);
int rpn_stack_push(RpnStack *stack, double value);
int rpn_stack_pop(RpnStack *stack, double *out);
int rpn_eval(const char *expr, double *out);

#endif
