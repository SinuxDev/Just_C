#include "csv.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *data;
    size_t len;
    size_t cap;
} Buf;

static int buf_push(Buf *buf, char ch) {
    if (buf->len + 1 > buf->cap) {
        size_t new_cap = buf->cap == 0 ? 64 : buf->cap * 2;
        char *grown = realloc(buf->data, new_cap);
        if (!grown) {
            return 0;
        }
        buf->data = grown;
        buf->cap = new_cap;
    }
    buf->data[buf->len++] = ch;
    return 1;
}

static char *buf_take(Buf *buf) {
    char *text = realloc(buf->data, buf->len + 1);
    if (!text) {
        free(buf->data);
        buf->data = NULL;
        buf->len = 0;
        buf->cap = 0;
        return NULL;
    }
    text[buf->len] = '\0';
    buf->data = NULL;
    buf->len = 0;
    buf->cap = 0;
    return text;
}

static void buf_free(Buf *buf) {
    free(buf->data);
    buf->data = NULL;
    buf->len = 0;
    buf->cap = 0;
}

static int row_add_cell(CsvRow *row, char *cell) {
    char **grown = realloc(row->cells, (size_t)(row->count + 1) * sizeof(char *));
    if (!grown) {
        return 0;
    }
    row->cells = grown;
    row->cells[row->count++] = cell;
    return 1;
}

static void row_free(CsvRow *row) {
    if (!row) {
        return;
    }
    for (int i = 0; i < row->count; i++) {
        free(row->cells[i]);
    }
    free(row->cells);
    row->cells = NULL;
    row->count = 0;
}

static int table_add_row(CsvTable *table, CsvRow *row) {
    CsvRow *grown = realloc(table->rows, (size_t)(table->row_count + 1) * sizeof(CsvRow));
    if (!grown) {
        return 0;
    }
    table->rows = grown;
    table->rows[table->row_count++] = *row;
    if (row->count > table->col_count) {
        table->col_count = row->count;
    }
    row->cells = NULL;
    row->count = 0;
    return 1;
}

void csv_free(CsvTable *table) {
    if (!table) {
        return;
    }
    for (int i = 0; i < table->row_count; i++) {
        row_free(&table->rows[i]);
    }
    free(table->rows);
    free(table);
}

static int finish_field(CsvRow *row, Buf *field) {
    char *cell = buf_take(field);
    if (!cell) {
        return 0;
    }
    if (!row_add_cell(row, cell)) {
        free(cell);
        return 0;
    }
    return 1;
}

static int finish_row(CsvTable *table, CsvRow *row, Buf *field, int *in_field) {
    if (*in_field) {
        if (!finish_field(row, field)) {
            return 0;
        }
        *in_field = 0;
    }
    if (row->count == 0) {
        return 1;
    }
    return table_add_row(table, row);
}

CsvTable *csv_parse(const char *text) {
    if (!text) {
        return NULL;
    }

    CsvTable *table = calloc(1, sizeof(CsvTable));
    if (!table) {
        return NULL;
    }

    CsvRow row = {0};
    Buf field = {0};
    int in_quotes = 0;
    int in_field = 0;
    int ok = 1;

    for (size_t i = 0; text[i] != '\0' && ok; i++) {
        char ch = text[i];

        if (in_quotes) {
            in_field = 1;
            if (ch == '"') {
                if (text[i + 1] == '"') {
                    ok = buf_push(&field, '"');
                    i++;
                } else {
                    in_quotes = 0;
                }
            } else {
                ok = buf_push(&field, ch);
            }
            continue;
        }

        if (ch == '"') {
            in_quotes = 1;
            in_field = 1;
            continue;
        }

        if (ch == ',') {
            ok = finish_field(&row, &field);
            in_field = 1;
            continue;
        }

        if (ch == '\n' || ch == '\r') {
            if (ch == '\r' && text[i + 1] == '\n') {
                i++;
            }
            ok = finish_row(table, &row, &field, &in_field);
            continue;
        }

        in_field = 1;
        ok = buf_push(&field, ch);
    }

    if (ok) {
        ok = finish_row(table, &row, &field, &in_field);
    }

    buf_free(&field);
    row_free(&row);

    if (!ok) {
        csv_free(table);
        return NULL;
    }

    return table;
}

CsvTable *csv_read_file(const char *path) {
    if (!path) {
        return NULL;
    }

    FILE *file = fopen(path, "rb");
    if (!file) {
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return NULL;
    }

    long size = ftell(file);
    if (size < 0) {
        fclose(file);
        return NULL;
    }

    if (fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return NULL;
    }

    char *text = malloc((size_t)size + 1);
    if (!text) {
        fclose(file);
        return NULL;
    }

    size_t read = fread(text, 1, (size_t)size, file);
    fclose(file);
    text[read] = '\0';

    CsvTable *table = csv_parse(text);
    free(text);
    return table;
}

void csv_print(const CsvTable *table) {
    if (!table || table->row_count == 0) {
        printf("(empty)\n");
        return;
    }

    int *widths = calloc((size_t)table->col_count, sizeof(int));
    if (!widths) {
        return;
    }

    for (int r = 0; r < table->row_count; r++) {
        for (int c = 0; c < table->rows[r].count; c++) {
            int len = (int)strlen(table->rows[r].cells[c]);
            if (len > widths[c]) {
                widths[c] = len;
            }
        }
    }

    for (int r = 0; r < table->row_count; r++) {
        for (int c = 0; c < table->col_count; c++) {
            const char *cell = "";
            if (c < table->rows[r].count && table->rows[r].cells[c]) {
                cell = table->rows[r].cells[c];
            }
            printf("%-*s", widths[c], cell);
            if (c + 1 < table->col_count) {
                printf("  |  ");
            }
        }
        printf("\n");

        if (r == 0 && table->row_count > 1) {
            for (int c = 0; c < table->col_count; c++) {
                for (int i = 0; i < widths[c]; i++) {
                    putchar('-');
                }
                if (c + 1 < table->col_count) {
                    printf("--+--");
                }
            }
            printf("\n");
        }
    }

    free(widths);
}
