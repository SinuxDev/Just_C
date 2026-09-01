#ifndef GREP_H
#define GREP_H

#include <stdio.h>

int grep_matches(const char *line, const char *pattern);
int grep_file(const char *path, const char *pattern, FILE *out);

#endif
