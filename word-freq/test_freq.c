#include "freq.h"

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

static void test_create_empty_table(void) {
    FreqTable *table = freq_table_create();

    EXPECT(table != NULL, "create returns a table");
    if (!table) {
        return;
    }

    EXPECT(table->buckets != NULL, "table has buckets");
    EXPECT(table->bucket_count > 0, "table has at least one bucket");
    EXPECT(freq_table_unique(table) == 0, "new table has no words");
    EXPECT(freq_table_get(table, "hello") == 0, "missing word count is zero");
    freq_table_free(table);
}

static void test_add_word_counts_and_lowercases(void) {
    FreqTable *table = freq_table_create();
    EXPECT(table != NULL, "create returns a table");
    if (!table) {
        return;
    }

    EXPECT(freq_table_add_word(table, "Hello") == 1, "first add succeeds");
    EXPECT(freq_table_add_word(table, "HELLO") == 1, "second add succeeds");
    EXPECT(freq_table_add_word(table, "world") == 1, "other word succeeds");
    EXPECT(freq_table_get(table, "hello") == 2, "hello counted twice");
    EXPECT(freq_table_get(table, "WORLD") == 1, "lookup is case-insensitive");
    EXPECT(freq_table_unique(table) == 2, "two unique words");

    freq_table_free(table);
}

static void test_add_word_rejects_bad_input(void) {
    FreqTable *table = freq_table_create();
    EXPECT(table != NULL, "create returns a table");
    if (!table) {
        return;
    }

    EXPECT(freq_table_add_word(table, NULL) == 0, "NULL word is rejected");
    EXPECT(freq_table_add_word(table, "") == 0, "empty word is rejected");
    EXPECT(freq_table_add_word(NULL, "hello") == 0, "NULL table is rejected");
    EXPECT(freq_table_unique(table) == 0, "rejected adds do not change the table");

    freq_table_free(table);
}

static void test_add_text_splits_on_punctuation(void) {
    FreqTable *table = freq_table_create();
    EXPECT(table != NULL, "create returns a table");
    if (!table) {
        return;
    }

    EXPECT(freq_table_add_text(table, "The cat sat on the mat.") == 1,
           "add_text succeeds");
    EXPECT(freq_table_get(table, "the") == 2, "the appears twice");
    EXPECT(freq_table_get(table, "cat") == 1, "cat appears once");
    EXPECT(freq_table_get(table, "mat") == 1, "mat has no period");
    EXPECT(freq_table_get(table, "on") == 1, "on appears once");
    EXPECT(freq_table_unique(table) == 5, "five unique words");

    freq_table_free(table);
}

static void test_sorted_by_count_then_word(void) {
    FreqTable *table = freq_table_create();
    EXPECT(table != NULL, "create returns a table");
    if (!table) {
        return;
    }

    freq_table_add_text(table, "be be be at as");

    size_t count = 0;
    FreqNode **sorted = freq_table_sorted(table, &count);
    EXPECT(sorted != NULL, "sorted returns an array");
    EXPECT(count == 3, "three unique words");
    if (sorted && count == 3) {
        EXPECT(strcmp(sorted[0]->word, "be") == 0, "highest count is first");
        EXPECT(sorted[0]->count == 3, "be counted three times");
        EXPECT(strcmp(sorted[1]->word, "as") == 0, "ties sort alphabetically");
        EXPECT(sorted[1]->count == 1, "as counted once");
        EXPECT(strcmp(sorted[2]->word, "at") == 0, "at follows as");
        EXPECT(sorted[2]->count == 1, "at counted once");
    }
    free(sorted);

    freq_table_free(table);
}

static void test_load_file(void) {
    const char *path = "test_words.txt";
    FILE *file = fopen(path, "w");
    EXPECT(file != NULL, "can write a temp file");
    if (!file) {
        return;
    }
    fputs("One fish, two fish.\nRed fish, blue fish.\n", file);
    fclose(file);

    FreqTable *table = freq_table_create();
    EXPECT(table != NULL, "create returns a table");
    if (!table) {
        remove(path);
        return;
    }

    EXPECT(freq_table_load_file(table, path) == 1, "load_file succeeds");
    EXPECT(freq_table_get(table, "fish") == 4, "fish counted four times");
    EXPECT(freq_table_get(table, "one") == 1, "one counted once");
    EXPECT(freq_table_get(table, "red") == 1, "red counted once");
    EXPECT(freq_table_unique(table) == 5, "five unique words");
    EXPECT(freq_table_load_file(table, "this-file-does-not-exist.txt") == 0,
           "missing file is rejected");

    freq_table_free(table);
    remove(path);
}

int main(void) {
    test_create_empty_table();
    test_add_word_counts_and_lowercases();
    test_add_word_rejects_bad_input();
    test_add_text_splits_on_punctuation();
    test_sorted_by_count_then_word();
    test_load_file();

    printf("%d passed, %d failed\n", g_passed, g_failed);
    return g_failed == 0 ? 0 : 1;
}
