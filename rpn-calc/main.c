#include "rpn.h"

#include <stdio.h>

static void print_usage(const char *prog) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s \"<expr>\"\n", prog);
    fprintf(stderr, "  %s \"3 4 + 2 *\"\n", prog);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        print_usage(argv[0]);
        return 1;
    }

    double value = 0;
    if (!rpn_eval(argv[1], &value)) {
        fprintf(stderr, "Could not evaluate '%s'.\n", argv[1]);
        return 1;
    }

    printf("%g\n", value);
    return 0;
}
