#ifndef FREQ_H
#define FREQ_H

#include <stddef.h>

typedef struct FreqNode {
    char *word;
    int count;
    struct FreqNode *next;
} FreqNode;

typedef struct {
    FreqNode **buckets;
    size_t bucket_count;
    size_t unique;
} FreqTable;

FreqTable *freq_table_create(void);
void freq_table_free(FreqTable *table);
int freq_table_add_word(FreqTable *table, const char *word);
int freq_table_get(const FreqTable *table, const char *word);
size_t freq_table_unique(const FreqTable *table);
int freq_table_add_text(FreqTable *table, const char *text);
int freq_table_load_file(FreqTable *table, const char *path);
FreqNode **freq_table_sorted(const FreqTable *table, size_t *out_count);

#endif
