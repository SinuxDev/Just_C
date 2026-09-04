#include "rpn.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_passed = 0;
static int g_failed = 0;

#define EXPECT(cond, msg)                                                      \
    do {                                                                       \
        if (cond) {                                                            \
            g_passed++;                                                        \
        } else {                                                               \
            g_failed++;                                                        \
            fprintf(stderr, "FAIL: %s (%s:%d)\n", msg, __FILE__, __LINE__);    \
        }                                                                      \
    } while (0)

static int nearly_equal(double a, double b) {
    return fabs(a - b) < 0.000001;
}

static void test_stack_push_pop(void) {
    RpnStack *stack = rpn_stack_create();
    EXPECT(stack != NULL, "create returns a stack");
    if (!stack) {
        return;
    }

    EXPECT(stack->size == 0, "new stack is empty");
    EXPECT(rpn_stack_push(stack, 3) == 1, "push 3");
    EXPECT(rpn_stack_push(stack, 4) == 1, "push 4");
    EXPECT(stack->size == 2, "stack has two values");

    double value = 0;
    EXPECT(rpn_stack_pop(stack, &value) == 1, "pop succeeds");
    EXPECT(nearly_equal(value, 4), "pop is last-in first-out");
    EXPECT(rpn_stack_pop(stack, &value) == 1, "pop second value");
    EXPECT(nearly_equal(value, 3), "bottom value is 3");
    EXPECT(rpn_stack_pop(stack, &value) == 0, "empty pop fails");

    rpn_stack_free(stack);
}

static void test_eval_add_mul(void) {
    double value = 0;
    EXPECT(rpn_eval("3 4 +", &value) == 1, "3 4 + succeeds");
    EXPECT(nearly_equal(value, 7), "3 4 + is 7");
    EXPECT(rpn_eval("3 4 + 2 *", &value) == 1, "3 4 + 2 * succeeds");
    EXPECT(nearly_equal(value, 14), "3 4 + 2 * is 14");
}

static void test_eval_sub_div(void) {
    double value = 0;
    EXPECT(rpn_eval("10 3 -", &value) == 1, "10 3 - succeeds");
    EXPECT(nearly_equal(value, 7), "10 3 - is 7");
    EXPECT(rpn_eval("20 4 /", &value) == 1, "20 4 / succeeds");
    EXPECT(nearly_equal(value, 5), "20 4 / is 5");
}

static void test_eval_rejects_bad_input(void) {
    double value = 99;
    EXPECT(rpn_eval("1 +", &value) == 0, "not enough operands");
    EXPECT(rpn_eval("1 0 /", &value) == 0, "divide by zero");
    EXPECT(rpn_eval("1 2", &value) == 0, "leftover values");
    EXPECT(rpn_eval("", &value) == 0, "empty expression");
    EXPECT(rpn_eval(NULL, &value) == 0, "NULL expression");
    EXPECT(rpn_eval("3 4 +", NULL) == 0, "NULL out");
    EXPECT(nearly_equal(value, 99), "failed eval does not write out");
}

int main(void) {
    test_stack_push_pop();
    test_eval_add_mul();
    test_eval_sub_div();
    test_eval_rejects_bad_input();

    printf("%d passed, %d failed\n", g_passed, g_failed);
    return g_failed == 0 ? 0 : 1;
}
