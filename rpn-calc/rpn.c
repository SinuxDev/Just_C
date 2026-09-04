#include "rpn.h"

#include <stdlib.h>
#include <string.h>

static int apply_op(RpnStack *stack, const char *op) {
    double right = 0;
    double left = 0;
    if (!rpn_stack_pop(stack, &right) || !rpn_stack_pop(stack, &left)) {
        return 0;
    }

    double result = 0;
    if (strcmp(op, "+") == 0) {
        result = left + right;
    } else if (strcmp(op, "-") == 0) {
        result = left - right;
    } else if (strcmp(op, "*") == 0) {
        result = left * right;
    } else if (strcmp(op, "/") == 0) {
        if (right == 0.0) {
            return 0;
        }
        result = left / right;
    } else {
        return 0;
    }

    return rpn_stack_push(stack, result);
}

RpnStack *rpn_stack_create(void) {
    return calloc(1, sizeof(RpnStack));
}

void rpn_stack_free(RpnStack *stack) {
    if (!stack) {
        return;
    }
    free(stack->items);
    free(stack);
}

int rpn_stack_push(RpnStack *stack, double value) {
    if (!stack) {
        return 0;
    }

    if (stack->size == stack->cap) {
        size_t cap = stack->cap ? stack->cap * 2 : 8;
        double *items = realloc(stack->items, cap * sizeof(double));
        if (!items) {
            return 0;
        }
        stack->items = items;
        stack->cap = cap;
    }

    stack->items[stack->size++] = value;
    return 1;
}

int rpn_stack_pop(RpnStack *stack, double *out) {
    if (!stack || !out || stack->size == 0) {
        return 0;
    }
    *out = stack->items[--stack->size];
    return 1;
}

int rpn_eval(const char *expr, double *out) {
    if (!expr || !out || expr[0] == '\0') {
        return 0;
    }

    char *copy = malloc(strlen(expr) + 1);
    if (!copy) {
        return 0;
    }
    memcpy(copy, expr, strlen(expr) + 1);

    RpnStack *stack = rpn_stack_create();
    if (!stack) {
        free(copy);
        return 0;
    }

    int ok = 1;
    for (char *token = strtok(copy, " \t\r\n"); token && ok;
         token = strtok(NULL, " \t\r\n")) {
        if (strcmp(token, "+") == 0 || strcmp(token, "-") == 0 ||
            strcmp(token, "*") == 0 || strcmp(token, "/") == 0) {
            ok = apply_op(stack, token);
            continue;
        }

        char *end = NULL;
        double value = strtod(token, &end);
        if (end == token || *end != '\0') {
            ok = 0;
            break;
        }
        ok = rpn_stack_push(stack, value);
    }

    double result = 0;
    if (!ok || stack->size != 1 || !rpn_stack_pop(stack, &result)) {
        rpn_stack_free(stack);
        free(copy);
        return 0;
    }

    *out = result;
    rpn_stack_free(stack);
    free(copy);
    return 1;
}
