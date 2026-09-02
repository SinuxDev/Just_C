#include "grep.h"

#include <stdio.h>

static void print_usage(const char *prog) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s <pattern> <file>\n", prog);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        print_usage(argv[0]);
        return 1;
    }

    int matches = grep_file(argv[2], argv[1], stdout);
    if (matches < 0) {
        fprintf(stderr, "Could not search '%s'.\n", argv[2]);
        return 1;
    }

    return matches > 0 ? 0 : 1;
}
