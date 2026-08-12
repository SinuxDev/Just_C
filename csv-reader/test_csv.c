#include "csv.h"

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

static void test_parse_simple_row(void) {
    CsvTable *table = csv_parse("name,age\n");

    EXPECT(table != NULL, "parse simple row returns a table");
    if (!table) {
        return;
    }

    EXPECT(table->row_count == 1, "simple CSV has one row");
    EXPECT(table->col_count == 2, "simple CSV has two columns");
    EXPECT(table->rows[0].count == 2, "first row has two cells");
    EXPECT(strcmp(table->rows[0].cells[0], "name") == 0, "first cell is name");
    EXPECT(strcmp(table->rows[0].cells[1], "age") == 0, "second cell is age");

    csv_free(table);
}

static void test_parse_multiple_rows(void) {
    CsvTable *table = csv_parse("a,b\n1,2\n3,4\n");

    EXPECT(table != NULL, "parse multiple rows returns a table");
    if (!table) {
        return;
    }

    EXPECT(table->row_count == 3, "CSV has three rows");
    EXPECT(table->col_count == 2, "CSV has two columns");
    EXPECT(strcmp(table->rows[1].cells[0], "1") == 0, "second row first cell");
    EXPECT(strcmp(table->rows[2].cells[1], "4") == 0, "third row second cell");

    csv_free(table);
}

static void test_parse_quoted_comma(void) {
    CsvTable *table = csv_parse("city,note\nYangon,\"Mandalay, Myanmar\"\n");

    EXPECT(table != NULL, "parse quoted comma returns a table");
    if (!table) {
        return;
    }

    EXPECT(table->row_count == 2, "quoted CSV has two rows");
    EXPECT(table->rows[1].count == 2, "quoted row still has two cells");
    EXPECT(strcmp(table->rows[1].cells[0], "Yangon") == 0, "unquoted city");
    EXPECT(strcmp(table->rows[1].cells[1], "Mandalay, Myanmar") == 0,
           "quoted field keeps the comma");

    csv_free(table);
}

static void test_parse_escaped_quotes(void) {
    CsvTable *table = csv_parse("title\n\"He said \"\"hello\"\"\"\n");

    EXPECT(table != NULL, "parse escaped quotes returns a table");
    if (!table) {
        return;
    }

    EXPECT(table->row_count == 2, "escaped-quote CSV has two rows");
    EXPECT(strcmp(table->rows[1].cells[0], "He said \"hello\"") == 0,
           "doubled quotes become a single quote");

    csv_free(table);
}

static void test_read_missing_file(void) {
    CsvTable *table = csv_read_file("this-file-does-not-exist.csv");
    EXPECT(table == NULL, "missing file returns NULL");
}

static void test_read_file(void) {
    const char *path = "test_data.csv";
    FILE *file = fopen(path, "w");
    EXPECT(file != NULL, "can create test_data.csv");
    if (!file) {
        return;
    }

    fputs("name,score\nAda,10\n", file);
    fclose(file);

    CsvTable *table = csv_read_file(path);
    EXPECT(table != NULL, "csv_read_file returns a table");
    if (table) {
        EXPECT(table->row_count == 2, "file CSV has two rows");
        EXPECT(strcmp(table->rows[1].cells[0], "Ada") == 0, "file row name");
        EXPECT(strcmp(table->rows[1].cells[1], "10") == 0, "file row score");
        csv_free(table);
    }

    remove(path);
}

int main(void) {
    test_parse_simple_row();
    test_parse_multiple_rows();
    test_parse_quoted_comma();
    test_parse_escaped_quotes();
    test_read_missing_file();
    test_read_file();

    printf("%d passed, %d failed\n", g_passed, g_failed);
    return g_failed == 0 ? 0 : 1;
}
