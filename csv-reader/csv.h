#ifndef CSV_H
#define CSV_H

typedef struct {
    char **cells;
    int count;
} CsvRow;

typedef struct {
    CsvRow *rows;
    int row_count;
    int col_count;
} CsvTable;

CsvTable *csv_parse(const char *text);
CsvTable *csv_read_file(const char *path);
void csv_print(const CsvTable *table);
void csv_free(CsvTable *table);

#endif
