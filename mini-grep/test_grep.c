#include "grep.h"

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

static void test_matches_substring(void) {
    EXPECT(grep_matches("hello world", "hello") == 1, "match at start");
    EXPECT(grep_matches("hello world", "world") == 1, "match at end");
    EXPECT(grep_matches("hello world", "lo wo") == 1, "match in the middle");
    EXPECT(grep_matches("hello world", "xyz") == 0, "missing pattern");
    EXPECT(grep_matches("Hello", "hello") == 0, "search is case-sensitive");
}

static void test_matches_rejects_bad_input(void) {
    EXPECT(grep_matches(NULL, "a") == 0, "NULL line");
    EXPECT(grep_matches("abc", NULL) == 0, "NULL pattern");
    EXPECT(grep_matches("abc", "") == 0, "empty pattern");
}

static void test_grep_file(void) {
    const char *src_path = "test_haystack.txt";
    const char *out_path = "test_grep_out.txt";
    FILE *src = fopen(src_path, "w");
    EXPECT(src != NULL, "can write haystack");
    if (!src) {
        return;
    }
    fputs("int main(void) {\n", src);
    fputs("    printf(\"hello\");\n", src);
    fputs("    return 0;\n", src);
    fputs("}\n", src);
    fclose(src);

    FILE *out = fopen(out_path, "w");
    EXPECT(out != NULL, "can write output");
    if (!out) {
        remove(src_path);
        return;
    }

    EXPECT(grep_file(src_path, "printf", out) == 1, "one matching line");
    fclose(out);

    FILE *in = fopen(out_path, "r");
    EXPECT(in != NULL, "can read output");
    if (in) {
        char line[128] = "";
        EXPECT(fgets(line, (int)sizeof(line), in) != NULL, "output has a line");
        EXPECT(strcmp(line, "    printf(\"hello\");\n") == 0, "printed matching line");
        EXPECT(fgets(line, (int)sizeof(line), in) == NULL, "no extra lines");
        fclose(in);
    }

    EXPECT(grep_file("missing-grep.txt", "hello", stdout) == -1,
           "missing file returns -1");
    EXPECT(grep_file(src_path, "", stdout) == -1, "empty pattern returns -1");

    remove(src_path);
    remove(out_path);
}

int main(void) {
    test_matches_substring();
    test_matches_rejects_bad_input();
    test_grep_file();

    printf("%d passed, %d failed\n", g_passed, g_failed);
    return g_failed == 0 ? 0 : 1;
}
