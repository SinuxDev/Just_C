#include "freq.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_usage(const char *prog) {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s <file>\n", prog);
    fprintf(stderr, "  %s -n <count> <file>\n", prog);
}

static int parse_count(const char *text, long *value) {
    char *end = NULL;
    long n = strtol(text, &end, 10);
    if (end == text || *end != '\0' || n <= 0) {
        return 0;
    }
    *value = n;
    return 1;
}

int main(int argc, char **argv) {
    const char *path = NULL;
    long top = 0;

    if (argc == 2) {
        path = argv[1];
    } else if (argc == 4 && strcmp(argv[1], "-n") == 0) {
        if (!parse_count(argv[2], &top)) {
            print_usage(argv[0]);
            return 1;
        }
        path = argv[3];
    } else {
        print_usage(argv[0]);
        return 1;
    }

    FreqTable *table = freq_table_create();
    if (!table) {
        fprintf(stderr, "Could not create frequency table.\n");
        return 1;
    }

    if (!freq_table_load_file(table, path)) {
        fprintf(stderr, "Could not read '%s'.\n", path);
        freq_table_free(table);
        return 1;
    }

    size_t count = 0;
    FreqNode **sorted = freq_table_sorted(table, &count);
    if (count == 0) {
        printf("(no words)\n");
        free(sorted);
        freq_table_free(table);
        return 0;
    }

    size_t shown = count;
    if (top > 0 && (size_t)top < shown) {
        shown = (size_t)top;
    }

    size_t width = 4;
    for (size_t i = 0; i < shown; i++) {
        size_t len = strlen(sorted[i]->word);
        if (len > width) {
            width = len;
        }
    }

    printf("%-*s  %s\n", (int)width, "word", "count");
    for (size_t i = 0; i < shown; i++) {
        printf("%-*s  %d\n", (int)width, sorted[i]->word, sorted[i]->count);
    }

    free(sorted);
    freq_table_free(table);
    return 0;
}
