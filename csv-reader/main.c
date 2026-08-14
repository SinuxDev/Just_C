#include "csv.h"

#include <stdio.h>

int main(int argc, char **argv) {
    const char *path = argc > 1 ? argv[1] : "sample.csv";
    CsvTable *table = csv_read_file(path);

    if (!table) {
        fprintf(stderr, "Could not read '%s'\n", path);
        fprintf(stderr, "Usage: csv_reader [file.csv]\n");
        return 1;
    }

    printf("File: %s  (%d rows, %d columns)\n\n", path, table->row_count,
           table->col_count);
    csv_print(table);
    csv_free(table);
    return 0;
}
