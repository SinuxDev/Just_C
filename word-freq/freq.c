#include "freq.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUCKET_COUNT 64
#define WORD_CAP 128

static char *normalize_word(const char *word) {
    if (!word || word[0] == '\0') {
        return NULL;
    }

    size_t len = strlen(word);
    char *out = malloc(len + 1);
    if (!out) {
        return NULL;
    }

    for (size_t i = 0; i < len; i++) {
        out[i] = (char)tolower((unsigned char)word[i]);
    }
    out[len] = '\0';
    return out;
}

static size_t hash_word(const char *word, size_t buckets) {
    unsigned long hash = 5381UL;
    for (const unsigned char *p = (const unsigned char *)word; *p; p++) {
        hash = ((hash << 5) + hash) + *p;
    }
    return (size_t)(hash % buckets);
}

static FreqNode *find_node(const FreqTable *table, const char *word) {
    size_t index = hash_word(word, table->bucket_count);
    for (FreqNode *node = table->buckets[index]; node; node = node->next) {
        if (strcmp(node->word, word) == 0) {
            return node;
        }
    }
    return NULL;
}

static int flush_word(FreqTable *table, char *word, size_t *length, size_t cap) {
    if (*length == 0 || *length >= cap) {
        *length = 0;
        return 1;
    }

    word[*length] = '\0';
    *length = 0;
    return freq_table_add_word(table, word);
}

static int take_char(FreqTable *table, int ch, char *word, size_t *length,
                     size_t cap) {
    if (isalnum((unsigned char)ch)) {
        if (*length + 1 < cap) {
            word[(*length)++] = (char)ch;
        } else {
            *length = cap;
        }
        return 1;
    }
    return flush_word(table, word, length, cap);
}

FreqTable *freq_table_create(void) {
    FreqTable *table = calloc(1, sizeof(FreqTable));
    if (!table) {
        return NULL;
    }

    table->buckets = calloc(BUCKET_COUNT, sizeof(FreqNode *));
    if (!table->buckets) {
        free(table);
        return NULL;
    }

    table->bucket_count = BUCKET_COUNT;
    return table;
}

void freq_table_free(FreqTable *table) {
    if (!table) {
        return;
    }

    if (table->buckets) {
        for (size_t i = 0; i < table->bucket_count; i++) {
            FreqNode *node = table->buckets[i];
            while (node) {
                FreqNode *next = node->next;
                free(node->word);
                free(node);
                node = next;
            }
        }
        free(table->buckets);
    }
    free(table);
}

int freq_table_add_word(FreqTable *table, const char *word) {
    if (!table || !word || word[0] == '\0') {
        return 0;
    }

    char *key = normalize_word(word);
    if (!key) {
        return 0;
    }

    FreqNode *existing = find_node(table, key);
    if (existing) {
        existing->count++;
        free(key);
        return 1;
    }

    FreqNode *node = calloc(1, sizeof(FreqNode));
    if (!node) {
        free(key);
        return 0;
    }

    node->word = key;
    node->count = 1;

    size_t index = hash_word(key, table->bucket_count);
    node->next = table->buckets[index];
    table->buckets[index] = node;
    table->unique++;
    return 1;
}

int freq_table_get(const FreqTable *table, const char *word) {
    if (!table || !word || word[0] == '\0') {
        return 0;
    }

    char *key = normalize_word(word);
    if (!key) {
        return 0;
    }

    FreqNode *node = find_node(table, key);
    int count = node ? node->count : 0;
    free(key);
    return count;
}

size_t freq_table_unique(const FreqTable *table) {
    return table ? table->unique : 0;
}

int freq_table_add_text(FreqTable *table, const char *text) {
    if (!table || !text) {
        return 0;
    }

    char word[WORD_CAP];
    size_t length = 0;
    for (const unsigned char *p = (const unsigned char *)text; *p; p++) {
        if (!take_char(table, *p, word, &length, sizeof(word))) {
            return 0;
        }
    }
    return flush_word(table, word, &length, sizeof(word));
}

int freq_table_load_file(FreqTable *table, const char *path) {
    if (!table || !path) {
        return 0;
    }

    FILE *file = fopen(path, "r");
    if (!file) {
        return 0;
    }

    char word[WORD_CAP];
    size_t length = 0;
    int ch = 0;
    int ok = 1;

    while (ok && (ch = fgetc(file)) != EOF) {
        ok = take_char(table, ch, word, &length, sizeof(word));
    }
    if (ok) {
        ok = flush_word(table, word, &length, sizeof(word));
    }

    fclose(file);
    return ok;
}

static int cmp_nodes(const void *left, const void *right) {
    const FreqNode *a = *(const FreqNode *const *)left;
    const FreqNode *b = *(const FreqNode *const *)right;
    if (a->count != b->count) {
        return (a->count > b->count) ? -1 : 1;
    }
    return strcmp(a->word, b->word);
}

FreqNode **freq_table_sorted(const FreqTable *table, size_t *out_count) {
    if (out_count) {
        *out_count = 0;
    }
    if (!table || !out_count || table->unique == 0) {
        return NULL;
    }

    FreqNode **list = malloc(table->unique * sizeof(FreqNode *));
    if (!list) {
        return NULL;
    }

    size_t n = 0;
    for (size_t i = 0; i < table->bucket_count; i++) {
        for (FreqNode *node = table->buckets[i]; node; node = node->next) {
            list[n++] = node;
        }
    }

    qsort(list, n, sizeof(FreqNode *), cmp_nodes);
    *out_count = n;
    return list;
}
