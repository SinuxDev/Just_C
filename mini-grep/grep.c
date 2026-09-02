#include "grep.h"

#include <stdio.h>
#include <string.h>

int grep_matches(const char *line, const char *pattern) {
    if (!line || !pattern || pattern[0] == '\0') {
        return 0;
    }
    return strstr(line, pattern) != NULL;
}

int grep_file(const char *path, const char *pattern, FILE *out) {
    if (!path || !pattern || pattern[0] == '\0' || !out) {
        return -1;
    }

    FILE *file = fopen(path, "r");
    if (!file) {
        return -1;
    }

    char line[512];
    int matches = 0;
    while (fgets(line, (int)sizeof(line), file)) {
        if (grep_matches(line, pattern)) {
            fputs(line, out);
            matches++;
        }
    }

    fclose(file);
    return matches;
}
